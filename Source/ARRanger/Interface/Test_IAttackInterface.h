//*************************************************
// �U�������邽�߂̃C���^�[�t�F�[�X
//*************************************************

#pragma once

/*
* Interface for implementing attacks
*/
class TEest_IAttackInterface
{
public:
	virtual ~TEest_IAttackInterface() = default;
 
	virtual void Attack() = 0;						// �U���̏������s���֐�
	virtual bool CanAttack() const = 0;				// �U���\���ǂ�����Ԃ��֐�
	virtual float GetAttackCoolTime() const = 0;	// �U���̃N�[���^�C�����擾����֐�
};

/*
* �L�����N�^�[�̑��ŕ����̍U���p�^�[����������������@�ōl�����C���^�[�t�F�[�X
* Attack() �� Player�N���X�� Attack(){Punch();Kick(); etc...}
* ��������́A�ǂ��ɂ�������.......
*/