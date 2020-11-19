//---------------------------------------------------------------------
//      AviUtl��Discord RPC��\������v���O�C��
//---------------------------------------------------------------------

#include <Windows.h>
#include "aviutl-sdk/filter.h"
#include "main.h"

typedef struct {
	short x, y;
} Vector2;
static void* buf0 = NULL;
static BOOL RPC_Enabled = TRUE;
static BOOL IS_SAVING = FALSE;

//---------------------------------------------------------------------
//		�t�B���^�\���̒�`
//---------------------------------------------------------------------
TCHAR   FILTER_NAME[]          = "AviUtl Discord RPC";
#define CHECK_NUM 1
TCHAR   CHECKBOX_NAME_0[]      = "�L���ɂ���";
TCHAR*  CHECKBOX_NAME          = CHECKBOX_NAME_0;
TCHAR** CHECKBOX_NAMES         = &CHECKBOX_NAME;
int     CHECKBOX_INITIAL_VAL[] = { 0 };
TCHAR   FILTER_INFO[]          = "";

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
	/*�O���֐��e�[�u��*/NULL,/*�V�X�e���g�p*/NULL,NULL,NULL,/*I�̂�*/NULL,
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
//		������
//---------------------------------------------------------------------
BOOL func_init(FILTER* fp) {
	return TRUE;
}

//---------------------------------------------------------------------
//		�I��
//---------------------------------------------------------------------
BOOL func_exit(FILTER* fp) {
	mem_free();
	return TRUE;
}

//---------------------------------------------------------------------
//		�ݒ�ύX
//---------------------------------------------------------------------
BOOL func_update(FILTER* fp, int status) {

}

//---------------------------------------------------------------------
//		�o�͊J�n
//---------------------------------------------------------------------
BOOL func_save_start(FILTER* fp, int s, int e, void* editP) {

}

//---------------------------------------------------------------------
//		�o�͏I��
//---------------------------------------------------------------------
BOOL func_save_end(FILTER* fp, void* editP) {

}

//---------------------------------------------------------------------
//		�E�C���h�E�^�C�g���ύX
//---------------------------------------------------------------------
BOOL func_modify_title(FILTER* fp, void* editP, int frame, LPSTR title, int max_title) {
	Display_RPC(fp, editP);
	return TRUE;
}

//---------------------------------------------------------------------
//		�E�C���h�E�v���V�[�W��
//---------------------------------------------------------------------
BOOL func_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, void* editPtr, FILTER* filterPtr) {
	switch (message) {
	case WM_PAINT:
	case WM_FILTER_CHANGE_EDIT:
		Display_RPC(filterPtr, editPtr);
		break;
	case WM_FILTER_CHANGE_WINDOW:
		if (filterPtr->exfunc->is_filter_window_disp(filterPtr)) {
			mem_alloc(filterPtr);
			return TRUE;
		}
		else {
			mem_free();
		}
		break;
	}
	return FALSE;
}

//---------------------------------------------------------------------
//		Discord RPC �ݒ�֐�
//---------------------------------------------------------------------
BOOL Display_RPC(FILTER* fp, void* editP) {

}