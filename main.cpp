//---------------------------------------------------------------------
//      AviUtl��Discord RPC��\������v���O�C��
//---------------------------------------------------------------------

// �C���N���[�h
#include <Windows.h>
#include <string>
#include <chrono>
#include <sstream>
#include <istream>
#include "aviutl-sdk/filter.h"
#include "main.h"
#include "discord-files/discord.h"
#include "strutil.h"

// �o�b�t�@�[
static void* buf0 = NULL;
// RPC�v���O�C�����L�����ǂ���
static BOOL RPC_Enabled = TRUE;
// �X�e�[�^�X
static int Status = NULL;
// ���ݕҏW���̃t�@�C����
static const char* FILTER_CURRENT_FILENAME = NULL;
// �^�C�}�[�̎��ʎq
static UINT_PTR timer_identifer = NULL;
// RPC���j������Ă��邩�ǂ���
static BOOL IS_Disposed = TRUE;
// �V�X�e�����
static LPSTR SYS_INFO_STR = NULL;

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
TCHAR   FILTER_INFO[]          = "AviUtl Discord RPC version 0.99c by mtripg6666tdr";

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
	NULL,
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
	NULL,
	// �o�͏I���֐�
	NULL,
	/*�O���֐��e�[�u��*/NULL,/*�V�X�e���g�p*/NULL,NULL,NULL,/*I�����v���O�C���̂�*/NULL,
	// �v���W�F�N�g�t�@�C���ۑ��֘A
	NULL, func_project_save,
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
//		������
//---------------------------------------------------------------------
#pragma optimize("", off)
BOOL func_init(FILTER* fp) {
	// �R�[���o�b�N�����s���邽�߂̃^�C�}�[��ݒ�
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
	SYS_INFO info;
	if (fp->exfunc->get_sys_info(NULL, &info)) {
		LPSTR ver = info.info;
		LPSTR app_name = "AviUtl ";
		LPSTR plugin_str = " (RPC powered by AviUtl RPC Plugin by mtripg6666tdr)";
		int num = strlen(ver) + strlen(app_name) + strlen(plugin_str) + 1;
		SYS_INFO_STR = new char[num];
		strcpy_s(SYS_INFO_STR, num, app_name);
		strcat_s(SYS_INFO_STR, num, ver);
		strcat_s(SYS_INFO_STR, num, plugin_str);
	}
	return TRUE;
}
#pragma optimize("", on)


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
		Initialize_RPC();
		Update_RPC(fp, NULL, Status, TRUE);
		break;
	case FILTER_CHECKBOX_STATUS_OFF:
		RPC_Enabled = FALSE;
		Dispose_RPC();
		break;
	}
	return TRUE;
}

//---------------------------------------------------------------------
//		�v���W�F�N�g�t�@�C���̕ۑ�
//---------------------------------------------------------------------
BOOL func_project_save(FILTER* fp, void* editp, void* data, int* size) {
	Update_RPC(fp, editp, RPC_STATUS_EDITING, FALSE);
	return FALSE;
}

//---------------------------------------------------------------------
//		�E�C���h�E�^�C�g���ύX
//---------------------------------------------------------------------
BOOL func_modify_title(FILTER* fp, void* editP, int frame, LPSTR title, int max_title) {
	return TRUE;
}

//---------------------------------------------------------------------
//		�^�C�}�[�e�B�b�N
//      �R�[���o�b�N�����s
//---------------------------------------------------------------------
void __stdcall func_timer_tick(HWND hWnd, UINT uMsg, UINT_PTR timer_id, DWORD dwTime) {
	core->RunCallbacks();
}

//---------------------------------------------------------------------
//		�E�C���h�E�v���V�[�W��
//---------------------------------------------------------------------
BOOL func_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, void* editPtr, FILTER* filterPtr) {
	if (core != NULL) {
		core->RunCallbacks();
	}

	switch (message) {
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
	case WM_FILTER_SAVE_END:
		Update_RPC(filterPtr, editPtr, RPC_STATUS_EDITING, TRUE);
		break;
	case WM_FILTER_FILE_CLOSE:
		Update_RPC(filterPtr, editPtr, RPC_STATUS_IDLING, TRUE);
		break;
	case WM_FILTER_SAVE_START:
		Update_RPC(filterPtr, editPtr, RPC_STATUS_SAVING, TRUE);
		break;
	}
	return FALSE;
}

//---------------------------------------------------------------------
//		Discord RPC �ݒ�֐�
//---------------------------------------------------------------------

// Discord�̃A�v���P�[�V�����̃N���C�A���gID
#define FILTER_RPC_CLIENT_ID 779296320019497020
BOOL Initialize_RPC() {
	if (!RPC_Enabled) {
		return false;
	}
	if (IS_Disposed) {
		discord::Core::Create(FILTER_RPC_CLIENT_ID, DiscordCreateFlags_NoRequireDiscord, &core);
		IS_Disposed = FALSE;
	}
	return TRUE;
}
BOOL Update_RPC(FILTER* filterPtr, void* editPtr, int status, bool isStart) {
	// �X�e�[�^�X��ۑ�
	Status = status;

	// �����Ȃ�I���
	if (!RPC_Enabled) {
		return FALSE;
	}
	if (!IS_Disposed) {
		std::string detail = "";
		if (editPtr != NULL && filterPtr != NULL) {
			FILE_INFO fi;
			SYS_INFO si;
			if (filterPtr->exfunc->get_sys_info(editPtr, &si) && 
				filterPtr->exfunc->get_file_info(editPtr, &fi)
				) {
				detail += fi.name;
				std::string filename;
				std::string rawFileName = std::string(si.project_name);
				if (rawFileName.find('\\') == std::string::npos) {
					filename = rawFileName;
				} else {
					std::stringstream ss{ rawFileName };
					std::string buf;
					while (std::getline(ss, buf, '\\')) {
						filename = buf;
					}
				}
				filename = multi_to_utf8_winapi(filename);
				detail += "(";
				detail += filename == "" ? u8"���ۑ�" : filename;
				detail += ")";
			}
		}

		std::string StateStr = "";
		// Ref: https://stackoverflow.com/questions/6012663/get-unix-timestamp-with-c
		switch (status) {
		case RPC_STATUS_SAVING:
			StateStr = u8"�G���R�[�h��";
			activity.GetAssets().SetSmallImage("status_encoding");
			activity.GetAssets().SetSmallText(u8"�G���R�[�h");
			break;
		case RPC_STATUS_IDLING:
			StateStr = u8"�A�C�h����";
			activity.GetAssets().SetSmallImage("status_idle");
			activity.GetAssets().SetSmallText(u8"�A�C�h�����");
			break;
		case RPC_STATUS_EDITING:
			StateStr = u8"�ҏW��";
			activity.GetAssets().SetSmallImage("status_editing");
			activity.GetAssets().SetSmallText(u8"�ҏW��");
			break;
		}
		activity.SetState(StateStr.c_str());
		activity.GetAssets().SetLargeImage("aviutl_icon_large");
		activity.GetAssets().SetLargeText(SYS_INFO_STR == NULL ? "AviUtl" : SYS_INFO_STR);
		if (isStart) {
			activity.GetTimestamps().SetStart(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
		}
		if (detail != "") {
			activity.SetDetails(detail.c_str());
		}

		core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
			
		});
	}
	else {
		Initialize_RPC();
	}
	return TRUE;
}
BOOL Dispose_RPC() {
	IS_Disposed = TRUE;
	core->ActivityManager().ClearActivity([](discord::Result result) {
	});
	return TRUE;
}