#include "gfx/CommandList/CommandList.h"
#include "gfx/Resource/Buffer.h"

namespace vg::gfx::dx12
{
	//--------------------------------------------------------------------------------------
	CommandQueue::CommandQueue(CommandQueueType _type) :
		base::CommandQueue(_type)
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
								 desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
								 desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		auto device = gfx::Device::get();
		auto * d3d12device = device->getd3d12Device();
		VG_VERIFY_SUCCEEDED(d3d12device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_d3d12commandQueue)));
	
        D3D12_QUERY_HEAP_DESC queryHeapDesc;
        queryHeapDesc.Count = MaxQueriesCount;
        queryHeapDesc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
        queryHeapDesc.NodeMask = 0;
        VG_VERIFY_SUCCEEDED(d3d12device->CreateQueryHeap(&queryHeapDesc, IID_PPV_ARGS(&m_d3d12queryHeap)));

        LARGE_INTEGER li;
        VG_VERIFY(QueryPerformanceFrequency(&li));

		m_frequencyCPU = li.QuadPart;
        m_d3d12commandQueue->GetTimestampFrequency((uint64_t *)&m_frequencyGPU);
        m_d3d12commandQueue->GetClockCalibration((uint64_t *)&m_timestampGPU, (uint64_t *)&m_timestampCPU);
	
		BufferDesc queryBufferDesc = BufferDesc(Usage::Staging, BindFlags::None, CPUAccessFlags::Read, BufferFlags::None, sizeof(core::u64), MaxQueriesCount);
		m_queryBuffer = device->createBuffer(queryBufferDesc, "QueryBuffer");

		for (auto & valid : m_isValidQuery)
			valid = false;
	}

	//--------------------------------------------------------------------------------------
	CommandQueue::~CommandQueue()
	{
		VG_SAFE_RELEASE(m_d3d12commandQueue);
		VG_SAFE_RELEASE(m_d3d12queryHeap);
		VG_SAFE_RELEASE(m_queryBuffer);
	}

    //--------------------------------------------------------------------------------------
	void CommandQueue::beginFrame(gfx::CommandList * _cmdList)
	{
		auto d3d12cmdList = ((ID3D12GraphicsCommandList *)_cmdList->getd3d12CommandList());

		// begin frame query
		d3d12cmdList->EndQuery(m_d3d12queryHeap, D3D12_QUERY_TYPE_TIMESTAMP, m_queryIndex+0);
		m_isValidQuery[m_queryIndex + 0] = true;
	}

    //--------------------------------------------------------------------------------------
    core::i64 CommandQueue::GetCPUTimestamp(core::i64 gpuTimestamp)
    {
        return (gpuTimestamp - m_timestampGPU) * m_frequencyCPU / m_frequencyGPU;
    }

    //--------------------------------------------------------------------------------------
	void CommandQueue::endFrame(gfx::CommandList * _cmdList)
	{
		auto d3d12cmdList = ((ID3D12GraphicsCommandList *)_cmdList->getd3d12CommandList());

        // end frame query
        d3d12cmdList->EndQuery(m_d3d12queryHeap, D3D12_QUERY_TYPE_TIMESTAMP, m_queryIndex + 1);
		m_isValidQuery[m_queryIndex + 1] = true;

		auto d3d12resource = m_queryBuffer->getResource().getd3d12BufferResource();

		auto readQueryIndex = (m_queryIndex + 2) % MaxQueriesCount;

		if (m_isValidQuery[readQueryIndex])
		{
			d3d12cmdList->ResolveQueryData(
				m_d3d12queryHeap,
				D3D12_QUERY_TYPE_TIMESTAMP,
				readQueryIndex,
				2,
				d3d12resource,
				readQueryIndex * sizeof(core::u64)
			);

			D3D12_RANGE range = { sizeof(uint64_t) * readQueryIndex, sizeof(uint64_t) * (readQueryIndex + 1) };
			core::u64 * pData = nullptr;
			core::u64 start, end;
			d3d12resource->Map(0, &range, (void**) &pData);
			{
				start = GetCPUTimestamp(pData[0]);
				end = GetCPUTimestamp(pData[1]);
			}
			d3d12resource->Unmap(0, 0);

			double enlapsedTime = double(end - start) / (((double)m_frequencyCPU) * 0.001);
			VG_DEBUGPRINT("GPU frame time = %.3f ms\n", enlapsedTime);
		}

		m_queryIndex = (m_queryIndex + 2) % MaxQueriesCount;
	}
}