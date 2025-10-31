#ifndef UI_MANAGER_H
#define UI_MANAGER_H

void ui_manager_init(void);
void ui_manager_show_welcome_screen(void);
void ui_manager_show_prompt_screen(const char* title, const char* message);
void ui_manager_show_message(const char* message);
void ui_manager_show_success_screen(const char* message);
void ui_manager_show_failure_screen(const char* message);

#endif // UI_MANAGER_H