#pragma once


// ���C���[
enum class LAYER
{
	BG,				// �w�i
	ITEM,			// �L�����N�^�[
	OBJ,			// �A�C�e��
	CHAR,			// �I�u�W�F
	MAX
};

LAYER begin(LAYER);
LAYER end(LAYER);

LAYER operator++(LAYER& key);
LAYER operator*(LAYER key);