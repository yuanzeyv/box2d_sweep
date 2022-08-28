#pragma once
//��ͼ������
#define PlottingScale (0.1f)						 //������, 0.1�� = 1��
#define DistanceTrans(meter) (meter * PlottingScale) //ʵ�ʾ���ת��Ϊ ������

enum ViewType {
	VIEW_TYPE_NULL = 0,	//����һ���յ�Ԫ
	MONSTER_HERO = 1,//�����
	HERO_MONSTER = 2,//Ӣ�۱� 
	HERO_HERO = 3,//Ӣ�۵�Ӣ��
	STATIC_HERO = 4,//��̬��Ӣ��
	HERO_STATIC = 5,//Ӣ�۵���̬
	HERO_BULLET = 6,//Ӣ�۵��ӵ�
	BULLET_HERO = 7,//�ӵ���Ӣ��
};
enum ViewStatus {
	VISIBLE = 0,
	INVISIBLE = 1,
};

//4����Ʒ����
enum BodyType {
	BODY_TYPE_NULL = 0,
	STATIC_BODY = 1,
	PLAYER_BODY = 2,
	MONSTER_BODY = 3,
	BULLET_BODY = 4,
};