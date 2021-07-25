#ifndef AVIUTL_DISCORD_RPC_MAIN
#define AVIUTL_DISCORD_RPC_MAIN
#include <Windows.h>
#include "aviutl-sdk/filter.h"

void mem_alloc(FILTER*);
void mem_free(void);
BOOL func_init(FILTER*);
BOOL func_exit(FILTER*);
BOOL func_update(FILTER*);
BOOL func_project_save(void* fp, void* editp, void* data, int* size);
BOOL func_modify_title(FILTER*);
BOOL func_WndProc(HWND, UINT, WPARAM, LPARAM, void*, FILTER*);
BOOL Initialize_RPC();
BOOL Dispose_RPC();
BOOL Update_RPC(FILTER*,void*, int, bool);
void __stdcall func_timer_tick(HWND, UINT, UINT_PTR, DWORD);

// �`�F�b�N�{�b�N�X�̃X�e�[�^�X���I�t
#define FILTER_CHECKBOX_STATUS_OFF 0
// �`�F�b�N�{�b�N�X�̃X�e�[�^�X���I��
#define FILTER_CHECKBOX_STATUS_ON 1

// �X�e�[�^�X��\���t���O
#define RPC_STATUS_EDITING 1
#define RPC_STATUS_IDLING 2
#define RPC_STATUS_SAVING 3

// �^�C�}�[�Ԋu
#define RPC_UPDATE_TICK 1000
#endif // !AVIUTL_DISCORD_RPC_MAIN
