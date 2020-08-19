#include <malloc.h>

#include <3ds.h>

#include "action.h"
#include "../resources.h"
#include "../task/uitask.h"
#include "../../core/core.h"

typedef struct {
    linked_list* items;
    list_item* selected;
} delete_system_save_data_data;

static void action_delete_system_save_data_draw_top(ui_view* view, void* data, float x1, float y1, float x2, float y2) {
    task_draw_system_save_data_info(view, ((delete_system_save_data_data*) data)->selected->data, x1, y1, x2, y2);
}

static void action_delete_system_save_data_update(ui_view* view, void* data, float* progress, char* text) {
    delete_system_save_data_data* deleteData = (delete_system_save_data_data*) data;

    system_save_data_info* info = (system_save_data_info*) deleteData->selected->data;

    FS_SystemSaveDataInfo sysInfo = {.mediaType = MEDIATYPE_NAND, .saveId = info->systemSaveDataId};
    Result res = FSUSER_DeleteSystemSaveData(sysInfo);

    ui_pop();
    info_destroy(view);

    if(R_FAILED(res)) {
        error_display_res(info, task_draw_system_save_data_info, res, "Failed to delete 系統儲存數據");
    } else {
        linked_list_remove(deleteData->items, deleteData->selected);
        task_free_system_save_data(deleteData->selected);

        prompt_display_notify("成功", "系統儲存數據已刪除", COLOR_TEXT, NULL, NULL, NULL);
    }

    free(data);
}

static void action_delete_system_save_data_onresponse(ui_view* view, void* data, u32 response) {
    if(response == PROMPT_YES) {
        info_display("正在刪除系統儲存數據中", "", false, data, action_delete_system_save_data_update, action_delete_system_save_data_draw_top);
    } else {
        free(data);
    }
}

void action_delete_system_save_data(linked_list* items, list_item* selected) {
    delete_system_save_data_data* data = (delete_system_save_data_data*) calloc(1, sizeof(delete_system_save_data_data));
    if(data == NULL) {
        error_display(NULL, NULL, "無法分配刪除系統儲存數據的數據");

        return;
    }

    data->items = items;
    data->selected = selected;

    prompt_display_yes_no("確認", "即將刪除選定的系統儲存數據，是否繼續？", COLOR_TEXT, data, action_delete_system_save_data_draw_top, action_delete_system_save_data_onresponse);
}