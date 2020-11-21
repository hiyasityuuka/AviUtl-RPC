//---------------------------------------------------------------------
//      AviUtl��Discord RPC��\������v���O�C��
//---------------------------------------------------------------------

// �C���N���[�h
#include <Windows.h>
#include <string>
#include "aviutl-sdk/filter.h"
#include "main.h"
#include "discord-files/discord.h"

typedef struct {
	short x, y;
} Vector2;

// �o�b�t�@�[
static void* buf0 = NULL;
// RPC�v���O�C�����L�����ǂ���
static BOOL RPC_Enabled = TRUE;
// �Z�[�u��(�G���R�[�h��)���ǂ���
static BOOL IS_SAVING = FALSE;
// ���ݕҏW���̃t�@�C����
static LPSTR FILE_NAME = NULL;
// �^�C�}�[�̎��ʎq
static UINT_PTR timer_identifer = NULL;
// RPC���j������Ă��邩�ǂ���
static BOOL IS_Disposed = TRUE;
// Discord Core
discord::Core* core{};
// Discord Acticity
discord::Activity activity{};

//---------------------------------------------------------------------
//		�t�B���^�\���̒�`
//---------------------------------------------------------------------
TCHAR   FILTER_NAME[]          = "AviUtl Discord RPC";
#define CHECK_NUM 1
TCHAR  *CHECKBOX_NAMES[]       = { "�L���ɂ���" };
int     CHECKBOX_INITIAL_VAL[] = { 0 };
TCHAR   FILTER_INFO[]          = "AviUtl Discord RPC Plugin version 0.99b by mtripg6666tdr";

FILTER_DLL filter = {
	// flag
	FILTER_FLAG_ALWAYS_ACTIVE | FILTER_FLAG_DISP_FILTER | FILTER_FLAG_EX_INFORMATION,
	// x, y
	NULL, NULL,
	// name
	FILTER_NAME,
	// Trackbar
	NULL, NULL, NULL, NULL, NULL,
	// Checkbox
	CHECK_NUM, CHECKBOX_NAMES, CHECKBOX_INITIAL_VAL,
	// func_proc: �t�B���^�����֐�
	func_proc,
	// func_init: �t�B���^�������֐�
	func_init,
	// func_exit: �t�B���^�I���֐�
	func_exit,
	// func_update: �t�B���^�ݒ�ύX�K�p�֐�
	func_update,
	// func_WndProc: �E�C���h�E�v���V�[�W��
	func_WndProc,
	// �V�X�e���g�p����
	NULL, NULL, 
	// �g���f�[�^�֘A
	NULL, NULL,
	// �g���v���O�C�����
	FILTER_INFO,
	// �o�͊J�n�֐�
	func_save_start,
	// �o�͏I���֐�
	func_save_end,
	/*�O���֐��e�[�u��*/NULL,/*�V�X�e���g�p*/NULL,NULL,NULL,/*I�����v���O�C���̂�*/NULL,
	// �v���W�F�N�g�t�@�C���ۑ��֘A
	NULL, NULL, 
	// �^�C�g���o�[�\���֐�
	func_modify_title,
	NULL, NULL
};

//---------------------------------------------------------------------
//		�t�B���^�\���̂̃|�C���^��n���֐�
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void) {
	return &filter;
}

//---------------------------------------------------------------------
//		�������Ǘ�
//---------------------------------------------------------------------
void mem_alloc(FILTER* fp) {
	int size;
	SYS_INFO si;
	fp->exfunc->get_sys_info(NULL, &si);
	size = si.vram_w * si.vram_h * 3;
	if (buf0 == NULL) {
		buf0 = VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
	}
}
void mem_free(void) {
	if (buf0) {
		VirtualFree(buf0, NULL, MEM_RELEASE);
		buf0 = NULL;
	}
}


//---------------------------------------------------------------------
//		�����֐�
//---------------------------------------------------------------------
BOOL func_proc(FILTER* fp, FILTER_PROC_INFO* fpip) {
	return TRUE;
}

//---------------------------------------------------------------------
//		������
//---------------------------------------------------------------------
BOOL func_init(FILTER* fp) {
//	MessageBox(NULL, "�e�X�g", "�e�X�g", MB_ICONINFORMATION);
	UINT_PTR timer = SetTimer(
		// �E�C���h�E�n���h��
		// *����̓v���O�C���̃E�C���h�E���\������Ă���Ƃ͌���Ȃ�����NULL
		NULL,
		// �^�C�}�[ID
		// ��Ɠ����B
		NULL,
		// �^�C�}�[�Ԋu
		RPC_UPDATE_TICK,
		// �R�[���o�b�N�֐�
		func_timer_tick
	);
	if (timer) {
		timer_identifer = timer;
	}
	else {
		throw;
	}
	return TRUE;
}

//---------------------------------------------------------------------
//		�I��
//---------------------------------------------------------------------
BOOL func_exit(FILTER* fp) {
	mem_free();
	KillTimer(NULL, timer_identifer);
	Dispose_RPC();
	return TRUE;
}

//---------------------------------------------------------------------
//		�ݒ�ύX
//---------------------------------------------------------------------
BOOL func_update(FILTER* fp, int status) {
	switch (fp->check[0]) {
	case FILTER_CHECKBOX_STATUS_ON:
		RPC_Enabled = TRUE;
		break;
	case FILTER_CHECKBOX_STATUS_OFF:
		RPC_Enabled = FALSE;
		break;
	}
	return TRUE;
}

//---------------------------------------------------------------------
//		�o�͊J�n
//---------------------------------------------------------------------
BOOL func_save_start(FILTER* fp, int s, int e, void* editP) {
	IS_SAVING = TRUE;
	return TRUE;
}

//---------------------------------------------------------------------
//		�o�͏I��
//---------------------------------------------------------------------
BOOL func_save_end(FILTER* fp, void* editP) {
	IS_SAVING = FALSE;
	return TRUE;
}

//---------------------------------------------------------------------
//		�E�C���h�E�^�C�g���ύX
//---------------------------------------------------------------------
BOOL func_modify_title(FILTER* fp, void* editP, int frame, LPSTR title, int max_title) {
	//Display_RPC(fp, editP);
	return TRUE;
}

//---------------------------------------------------------------------
//		�^�C�}�[�e�B�b�N
//---------------------------------------------------------------------
void __stdcall func_timer_tick(HWND hWnd, UINT uMsg, UINT_PTR timer_id, DWORD dwTime) {
	Display_RPC(NULL, NULL);
}

//---------------------------------------------------------------------
//		�E�C���h�E�v���V�[�W��
//---------------------------------------------------------------------
BOOL func_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, void* editPtr, FILTER* filterPtr) {
	FILE_INFO* fi = NULL;

	switch (message) {
	/*case WM_FILTER_CHANGE_EDIT:
		core->RunCallbacks();
		break;*/
	case WM_FILTER_CHANGE_WINDOW:
		if (filterPtr->exfunc->is_filter_window_disp(filterPtr)) {
			mem_alloc(filterPtr);
			return TRUE;
		}
		else {
			mem_free();
		}
		break;
	case WM_FILTER_FILE_OPEN:
		if (filterPtr->exfunc->get_file_info(editPtr, fi)) {
			FILE_NAME = fi->name;
		}
		else {
			FILE_NAME = NULL;
		}
		break;
	case WM_FILTER_FILE_CLOSE:
		FILE_NAME = NULL;
		core->RunCallbacks();
		break;
	}
	return FALSE;
}

//---------------------------------------------------------------------
//		Discord RPC �ݒ�֐�
//---------------------------------------------------------------------
#define FILTER_RPC_CLIENT_ID 779296320019497020
BOOL Initialize_RPC() {
	if (!RPC_Enabled) {
		return false;
	}
	if (IS_Disposed) {
		IS_Disposed = FALSE;
		discord::Core::Create(FILTER_RPC_CLIENT_ID, DiscordCreateFlags_NoRequireDiscord, &core);
		activity.SetState(u8"�N����");
		//activity.SetDetails("");
		core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
			
		});
		Display_RPC(NULL, NULL);
	}
	else {
		throw;
	}
	return TRUE;
}
BOOL Display_RPC(FILTER* fp, void* editPtr) {
	if (!RPC_Enabled) {
		return FALSE;
	}
	if (!IS_Disposed) {
		std::string State = "";
		if (IS_SAVING) {
			State = u8"�G���R�[�h��";
		}else if (FILE_NAME == NULL) {
			State = u8"�A�C�h����";
		}
		else {
			State = u8"�ҏW��";
		}
		activity.SetState(State.c_str());
		if (FILE_NAME != NULL) {
			activity.SetDetails(FILE_NAME);
		}
		core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
			
		});
		core->RunCallbacks();
	}
	else {
		Initialize_RPC();
	}
	return TRUE;
}
BOOL Dispose_RPC() {
	IS_Disposed = TRUE;
	return TRUE;
}