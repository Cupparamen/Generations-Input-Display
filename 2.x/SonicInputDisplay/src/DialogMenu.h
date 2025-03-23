#ifndef DIALOG_MENU_H
#define DIALOG_MENU_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <string>
#include <vector>

enum SettingType { SET_BOOL, SET_INT, SET_FLOAT };

struct SettingField {
    std::string key;      // Key name in settings.ini
    std::string label;    // Descriptive text for the control
    SettingType type;
    std::string valueStr; // Editable value (stored as text)
};

class DialogMenu {
public:
    DialogMenu();
    ~DialogMenu();

    // Initialize the dialog menu with a TTF_Font pointer.
    void init(TTF_Font* font);

    // Open or close the dialog window.
    void setActive(bool active);
    bool isActive() const;

    // Handle events for the dialog window.
    void handleEvent(const SDL_Event& e);

    // Render the entire dialog window.
    void render();

    // Apply the changed settings to the global configuration.
    bool applyChanges();

    // Reload fields from the current global settings.
    void refreshFields();

    // Helper getters.
    int   getInt(const std::string& key, int defaultVal);
    float getFloat(const std::string& key, float defaultVal);
    bool  getBool(const std::string& key, bool defaultVal);

    // Return the SDL_Window* for this dialog.
    SDL_Window* getWindow() const { return dialogWindow; }

private:
    bool active;
    int selectedField; // The index (0..N-1) of the currently selected control.
    std::vector<SettingField> fields; // All controls in the desired order.
    std::vector<SDL_Rect> fieldRects;
    // Returns the dynamic color for a given field key.
    SDL_Color getDynamicColorForField(const std::string& key);

    TTF_Font* font;

    // Our separate settings window and its renderer.
    SDL_Window* dialogWindow;
    SDL_Renderer* dialogRenderer;

    // The options panel (the main portion of the dialog window).
    SDL_Rect optionsRect;

    SDL_Color bgColor;
    SDL_Color borderColor;

    // Render a single control. The given rectangle is split into two parts:
    // the left portion displays the label (with dynamic coloring for keywords),
    // and the right portion displays the editable value.
    void renderField(SDL_Renderer* renderer, const SettingField& field, const SDL_Rect& rect, bool selected);

    // Helper conversion functions.
    int parseInt(const std::string& s, int defaultVal);
    float parseFloat(const std::string& s, float defaultVal);
    bool parseBool(const std::string& s, bool defaultVal);
};

#endif // DIALOG_MENU_H
