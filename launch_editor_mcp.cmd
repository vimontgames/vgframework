@echo off
REM Lance l'editeur vgframework avec le bridge MCP actif.
REM La variable DOIT etre posee dans le process qui lance l'exe.

cd /d "%~dp0"
set "VG_MCP_BRIDGE=1"

echo [launch_editor_mcp] VG_MCP_BRIDGE=%VG_MCP_BRIDGE%
echo [launch_editor_mcp] cwd = %CD%
echo [launch_editor_mcp] demarrage de editor.exe ...

start "" "%~dp0editor.exe"
