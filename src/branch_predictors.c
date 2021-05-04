//
// This file contains all of the implementations of the branch predictor
// constructors from the branch_predictors.h file.
//
// It also contains stubs of all of the functions that are added to each
// branch_predictor struct at construction time.
//
// Feel free to create any additional structs or global variables that you
// want.
//
// ============================================================================
// NOTE: It is recommended that you read the comments in the
// branch_predictors.h file for further context on what each function is for.
// ============================================================================
//

#include "branch_predictors.h"

// ANT Branch Predictor
// ============================================================================

enum branch_direction ant_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                   uint32_t address)
{
    // Always do not take
    return NOT_TAKEN;
}

void ant_branch_predictor_handle_result(struct branch_predictor *branch_predictor, uint32_t address,
                                        enum branch_direction branch_direction)
{
    // Do not need for ANT
}

void ant_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    //no necessary cleanup
}

struct branch_predictor *ant_branch_predictor_new(uint32_t num_branches,
                                                  struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *ant_bp = calloc(1, sizeof(struct branch_predictor));
    ant_bp->cleanup = &ant_branch_predictor_cleanup;
    ant_bp->predict = &ant_branch_predictor_predict;
    ant_bp->handle_result = &ant_branch_predictor_handle_result;

    return ant_bp;
}

// AT Branch Predictor
// ============================================================================

enum branch_direction at_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                  uint32_t address)
{
    // TAlways take
    return TAKEN;
}

void at_branch_predictor_handle_result(struct branch_predictor *branch_predictor, uint32_t address,
                                       enum branch_direction branch_direction)
{
    // Not needed for AT
}

void at_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    // No necessary cleanup
}

struct branch_predictor *at_branch_predictor_new(uint32_t num_branches,
                                                 struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *at_bp = calloc(1, sizeof(struct branch_predictor));
    at_bp->cleanup = &at_branch_predictor_cleanup;
    at_bp->predict = &at_branch_predictor_predict;
    at_bp->handle_result = &at_branch_predictor_handle_result;

    return at_bp;
}

// BTFNT Branch Predictor
// ============================================================================

enum branch_direction btfnt_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                     uint32_t address)
{
    // return this branch predictors prediction for the branch at the
    // given address.
    uint32_t i = 0;
    while(branch_predictor->data[i].address != NULL) {
        if(branch_predictor->data[i].address == address){
            if(branch_predictor->data[i].target > address) {
                return NOT_TAKEN;
            } else {
                return TAKEN;
            }
        }
        i++;
    }
}

void btfnt_branch_predictor_handle_result(struct branch_predictor *branch_predictor,
                                          uint32_t address, enum branch_direction branch_direction)
{
    // use this function to update the state of the branch predictor
    // given the most recent branch direction.
}

void btfnt_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    // cleanup data
    free(branch_predictor->data);
}

struct branch_predictor *btfnt_branch_predictor_new(uint32_t num_branches,
                                                    struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *btfnt_bp = calloc(1, sizeof(struct branch_predictor));
    btfnt_bp->cleanup = &btfnt_branch_predictor_cleanup;
    btfnt_bp->predict = &btfnt_branch_predictor_predict;
    btfnt_bp->handle_result = &btfnt_branch_predictor_handle_result;

    // allocate storage for any data necessary for this branch predictor
    btfnt_bp->data = calloc(num_branches, sizeof(struct branch_metadata));
    btfnt_bp->data = branch_metadatas;

    return btfnt_bp;
}

// LTG Branch Predictor
// ============================================================================

enum branch_direction ltg_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                   uint32_t address)
{
    // return this branch predictors prediction for the branch at the
    // given address.
    uint32_t index = branch_predictor->history_register;
    if(branch_predictor->pht[index] == 0) {
        return NOT_TAKEN;
    } else {
        return TAKEN;
    }

}

void ltg_branch_predictor_handle_result(struct branch_predictor *branch_predictor, uint32_t address,
                                        enum branch_direction branch_direction)
{
    // use this function to update the state of the branch predictor
    // given the most recent branch direction.
    uint32_t index = branch_predictor->history_register;
    if (branch_direction == TAKEN) {
        branch_predictor->pht[index] = 1;
        branch_predictor->history_register = (index << 1) | 0b00000000000000000000000000000001;
        index = branch_predictor->history_register;
        branch_predictor->history_register = index & 0b00000000000000000000000000011111;
    } else {
        branch_predictor->pht[index] = 0;
        branch_predictor->history_register = (index << 1) & 0b00000000000000000000000000011111;
    }
}

void ltg_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    // cleanup
    free(branch_predictor->pht);
}

struct branch_predictor *ltg_branch_predictor_new(uint32_t num_branches,
                                                  struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *ltg_bp = calloc(1, sizeof(struct branch_predictor));
    ltg_bp->cleanup = &ltg_branch_predictor_cleanup;
    ltg_bp->predict = &ltg_branch_predictor_predict;
    ltg_bp->handle_result = &ltg_branch_predictor_handle_result;

    // allocate storage for any data necessary for this branch predictor
    ltg_bp->history_register = 0;
    ltg_bp->pht = calloc(32, sizeof(uint32_t));
    for(uint8_t i = 0; i < 32; i++) {
        ltg_bp->pht[i] = 0;
    }

    return ltg_bp;
}

// LTL Branch Predictor
// ============================================================================

enum branch_direction ltl_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                   uint32_t address)
{
    // return this branch predictors prediction for the branch at the
    // given address.
    uint32_t index = address & 0b00000000000000000000000000001111;
    uint32_t value = branch_predictor->history_register[index - 1];
    if(branch_predictor->pht[(index-1)*16 + value] == 0) {
        return NOT_TAKEN;
    } else {
        return TAKEN;
    }
}

void ltl_branch_predictor_handle_result(struct branch_predictor *branch_predictor, uint32_t address,
                                        enum branch_direction branch_direction)
{
    // use this function to update the state of the branch predictor
    // given the most recent branch direction.
    uint32_t index = address & 0b00000000000000000000000000001111;
    uint32_t value = branch_predictor->history_register[index - 1];
    if (branch_direction == TAKEN) {
        branch_predictor->pht[(index-1)*16 + value] = 1;
        branch_predictor->history_register[index - 1] = (value << 1) | 0b00000000000000000000000000000001;
        value = branch_predictor->history_register[index - 1];
        branch_predictor->history_register[index - 1] = value & 0b00000000000000000000000000001111;
    } else {
        branch_predictor->pht[(index-1)*16 + value] = 0;
        branch_predictor->history_register[index - 1] = (value << 1) & 0b00000000000000000000000000001111;
    }
}

void ltl_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    // cleanup 
    free(branch_predictor->pht);
    free(branch_predictor->history_register);
}

struct branch_predictor *ltl_branch_predictor_new(uint32_t num_branches,
                                                  struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *ltl_bp = calloc(1, sizeof(struct branch_predictor));
    ltl_bp->cleanup = &ltl_branch_predictor_cleanup;
    ltl_bp->predict = &ltl_branch_predictor_predict;
    ltl_bp->handle_result = &ltl_branch_predictor_handle_result;

    // allocate storage for any data necessary for this branch predictor
    ltl_bp->history_register = calloc(16, sizeof(uint32_t));
    ltl_bp->pht = calloc(16*16, sizeof(uint32_t));
    for(uint32_t i = 0; i < 256; i++) {
        if(i < 16) {
            ltl_bp->history_register[i] = 0;
        }
        ltl_bp->pht[i] = 0;
    }

    return ltl_bp;
}

// 2BG Branch Predictor
// ============================================================================

enum branch_direction tbg_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                   uint32_t address)
{
    // return this branch predictors prediction for the branch at the
    // given address.
    uint32_t index = branch_predictor->history_register;
    if(branch_predictor->pht[index] == 0 || branch_predictor->pht[index] == 1) {
        return NOT_TAKEN;
    } else {
        return TAKEN;
    }
}

void tbg_branch_predictor_handle_result(struct branch_predictor *branch_predictor, uint32_t address,
                                        enum branch_direction branch_direction)
{
    // use this function to update the state of the branch predictor
    // given the most recent branch direction.
    uint32_t index = branch_predictor->history_register;
    if (branch_direction == TAKEN) {
        if(branch_predictor->pht[index] != 3) {
            branch_predictor->pht[index]++;
        }
        branch_predictor->history_register = (index << 1) | 0b00000000000000000000000000000001;
        index = branch_predictor->history_register;
        branch_predictor->history_register = index & 0b00000000000000000000000000011111;
    } else {
        if(branch_predictor->pht[index] != 0) {
            branch_predictor->pht[index]--;
        }
        branch_predictor->history_register = (index << 1) & 0b00000000000000000000000000011111;
    }
}

void tbg_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    // TODO cleanup if necessary
    free(branch_predictor->pht);
}

struct branch_predictor *tbg_branch_predictor_new(uint32_t num_branches,
                                                  struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *tbg_bp = calloc(1, sizeof(struct branch_predictor));
    tbg_bp->cleanup = &tbg_branch_predictor_cleanup;
    tbg_bp->predict = &tbg_branch_predictor_predict;
    tbg_bp->handle_result = &tbg_branch_predictor_handle_result;

    // allocate storage for any data necessary for this branch predictor
    tbg_bp->history_register = 0;
    tbg_bp->pht = calloc(32, sizeof(uint32_t));
    for(uint8_t i = 0; i < 32; i++) {
        tbg_bp->pht[i] = 0;
    }

    return tbg_bp;
}

// 2BL Branch Predictor
// ============================================================================

enum branch_direction tbl_branch_predictor_predict(struct branch_predictor *branch_predictor,
                                                   uint32_t address)
{
    // return this branch predictors prediction for the branch at the
    // given address.
    uint32_t index = address & 0b00000000000000000000000000001111;
    uint32_t value = branch_predictor->history_register[index - 1];
    if(branch_predictor->pht[(index-1)*16 + value] == 0 || branch_predictor->pht[(index-1)*16 + value] == 1) {
        return NOT_TAKEN;
    } else {
        return TAKEN;
    }
}

void tbl_branch_predictor_handle_result(struct branch_predictor *branch_predictor, uint32_t address,
                                        enum branch_direction branch_direction)
{
    // use this function to update the state of the branch predictor
    // given the most recent branch direction.
    uint32_t index = address & 0b00000000000000000000000000001111;
    uint32_t value = branch_predictor->history_register[index - 1];
    if (branch_direction == TAKEN) {
        if(branch_predictor->pht[(index-1)*16 + value] != 3) {
            branch_predictor->pht[(index-1)*16 + value]++;
        }
        branch_predictor->history_register[index - 1] = (value << 1) | 0b00000000000000000000000000000001;
        value = branch_predictor->history_register[index - 1];
        branch_predictor->history_register[index - 1] = value & 0b00000000000000000000000000001111;
    } else {
        if(branch_predictor->pht[(index-1)*16 + value] != 0) {
            branch_predictor->pht[(index-1)*16 + value]--;
        }
        branch_predictor->history_register[index - 1] = (value << 1) & 0b00000000000000000000000000001111;
    }
}

void tbl_branch_predictor_cleanup(struct branch_predictor *branch_predictor)
{
    // cleanup
    free(branch_predictor->history_register);
    free(branch_predictor->pht);
}

struct branch_predictor *tbl_branch_predictor_new(uint32_t num_branches,
                                                  struct branch_metadata *branch_metadatas)
{
    struct branch_predictor *tbl_bp = calloc(1, sizeof(struct branch_predictor));
    tbl_bp->cleanup = &tbl_branch_predictor_cleanup;
    tbl_bp->predict = &tbl_branch_predictor_predict;
    tbl_bp->handle_result = &tbl_branch_predictor_handle_result;

    // allocate storage for any data necessary for this branch predictor
    tbl_bp->history_register = calloc(16, sizeof(uint32_t));
    tbl_bp->pht = calloc(16*16, sizeof(uint32_t));
    for(uint32_t i = 0; i < 256; i++) {
        if(i < 16) {
            tbl_bp->history_register[i] = 0;
        }
        tbl_bp->pht[i] = 0;
    }

    return tbl_bp;
}
