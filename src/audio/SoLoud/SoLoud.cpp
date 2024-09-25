#include "audio/Precomp.h"
#include "SoLoud.h"

//--------------------------------------------------------------------------------------
// All available audio backends
//--------------------------------------------------------------------------------------
#define AUDIOBACKED_NULL       0
#define AUDIOBACKEND_MINIAUDIO 1

//--------------------------------------------------------------------------------------
// Current audio backend
//--------------------------------------------------------------------------------------
#define AUDIOBACKEND AUDIOBACKEND_MINIAUDIO

#if AUDIOBACKEND == AUDIOBACKED_NULL
#define WITH_NULL
#include "soloud/src/backend/null/soloud_null.cpp"
#elif AUDIOBACKEND == AUDIOBACKEND_MINIAUDIO
#define WITH_MINIAUDIO
#include "soloud/src/backend/miniaudio/soloud_miniaudio.cpp"
#endif

#include "soloud/src/core/soloud.cpp"
#include "soloud/src/core/soloud_audiosource.cpp"
#include "soloud/src/core/soloud_bus.cpp"
#include "soloud/src/core/soloud_core_3d.cpp"
#include "soloud/src/core/soloud_core_basicops.cpp"
#include "soloud/src/core/soloud_core_faderops.cpp"
#include "soloud/src/core/soloud_core_filterops.cpp"
#include "soloud/src/core/soloud_core_getters.cpp"
#include "soloud/src/core/soloud_core_setters.cpp"
#include "soloud/src/core/soloud_core_voicegroup.cpp"
#include "soloud/src/core/soloud_core_voiceops.cpp"
#include "soloud/src/core/soloud_fader.cpp"
#include "soloud/src/core/soloud_fft.cpp"
#include "soloud/src/core/soloud_fft_lut.cpp"
#include "soloud/src/core/soloud_file.cpp"
#include "soloud/src/core/soloud_filter.cpp"
#include "soloud/src/core/soloud_misc.cpp"
#include "soloud/src/core/soloud_queue.cpp"
#include "soloud/src/core/soloud_thread.cpp"

#include "soloud/src/audiosource/speech/darray.cpp"
#include "soloud/src/audiosource/speech/klatt.cpp"
#include "soloud/src/audiosource/speech/resonator.cpp"
#include "soloud/src/audiosource/speech/soloud_speech.cpp"
#include "soloud/src/audiosource/speech/tts.cpp"

#include "soloud/src/audiosource/wav/stb_vorbis.c"
#include "soloud/src/audiosource/wav/dr_impl.cpp"
#include "soloud/src/audiosource/wav/soloud_wav.cpp"
#include "soloud/src/audiosource/wav/soloud_wavstream.cpp"