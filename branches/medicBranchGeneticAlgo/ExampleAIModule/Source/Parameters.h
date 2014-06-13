#pragma once

enum Params{
	S_GATHER_MINERALS,
	S_BUILD_BARRACKS_DENOMINATOR,
	S_BUILD_CMD_CENTER,
	S_ATTACK_NEAR,
	S_ATTACK_MID,
	S_ATTACK_FAR,
	S_TRAIN_SCV_DENOMINATOR,
	S_TRAIN_MEDIC_RATIO,
	S_TRAIN_MARINE,
	K_SCV_GATHER_MINERALS,
	K_SCV_REPAIR_NEAR,
	K_SCV_REPAIR_MID,
	K_SCV_REPAIR_FAR,
	K_SCV_EXPLORE,
	K_SCV_ATTACK_NEAR,
	K_SCV_ATTACK_MID,
	K_SCV_ATTACK_FAR,
	K_MARINE_EXPLORE,
	K_MARINE_ATTACK_NEAR,
	K_MARINE_ATTACK_MID,
	K_MARINE_ATTACK_FAR,
	K_MARINE_ATTACK_FAR,
	K_GENERAL_TRAIN_SCV_DENOMINATOR,
	K_GENERAL_TRAIN_MARINE,
	K_GENERAL_TRAIN_MEDIC_RATIO,
	M_PACK_SIZE
};

class Parameters
{
public:
	Parameters(void);
	~Parameters(void);
};

/*'s_gather_minerals', 's_build_barracks_denominator', 's_build_cmd_center',
        's_attack_near', 's_attack_mid', 's_attack_far', 's_train_scv_denominator',
        's_train_medic_ratio',
        's_train_marine', 'k_scv_gather_minerals', 'k_scv_build_barracks', 'k_scv_build_supply',
        'k_scv_build_supply', 'k_scv_build_cmd_center', 'k_scv_repair_near', 'k_scv_repair_mid',
        'k_scv_repair_far', 'k_scv_explore', 'k_scv_attack_near', 'k_scv_repair_mid',
        'k_scv_repair_far', 'k_scv_explore', 'k_scv_attack_near', 'k_scv_attack_mid',
        'k_scv_attack_far', 'k_marine_explore', 'k_marine_attack_near', 'k_marine_attack_mid',
        'k_marine_attack_far', 'k_general_train_scv_denominator', 'k_general_train_marine',
        'k_general_train_medic_denominator',
        'm_pack_size'
		*/
