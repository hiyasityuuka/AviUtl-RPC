#ifndef AVIUTL_DISCORD_RPC_MAIN
#define AVIUTL_DISCORD_RPC_MAIN
#include <Windows.h>
#include "aviutl-sdk/filter.h"
//EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void);
void mem_alloc(FILTER*);
void mem_free(void);
BOOL func_init(FILTER*);
BOOL func_exit(FILTER*);
BOOL func_update(FILTER*);
BOOL func_save_start(FILTER*);
BOOL func_save_end(FILTER*);
BOOL func_modify_title(FILTER*);
BOOL func_WndProc(HWND, UINT, WPARAM, LPARAM, void*, FILTER*);
BOOL Initialize_RPC();
BOOL Display_RPC(FILTER*, void*);
BOOL Dispose_RPC();
BOOL Update_RPC_Flags(FILTER*, void*);
void __stdcall func_timer_tick(HWND, UINT, UINT_PTR, DWORD);
// �`�F�b�N�{�b�N�X�̃X�e�[�^�X���I�t
#define FILTER_CHECKBOX_STATUS_OFF 0
// �`�F�b�N�{�b�N�X�̃X�e�[�^�X���I��
#define FILTER_CHECKBOX_STATUS_ON 1
// �^�C�}�[�Ԋu
#define RPC_UPDATE_TICK 5000
#endif // !AVIUTL_DISCORD_RPC_MAIN
