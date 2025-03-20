#include "DialogMenu.h"
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cstdio> // For snprintf

// --- Extern Globals (these must be defined in one .cpp file in your project) ---
extern bool showCoordinatesSwitch;
extern bool showMSpeedSwitch;
extern bool toonSwitch;
extern bool dPadSwitch;
extern int circleSize;
extern SDL_Color joystickCircleColor;
extern bool staticOverlayDirty;

extern float pulseRate;
extern int pulseAmplitude;
extern int glowMinAlpha;
extern SDL_Color joystickGlowColor;
extern SDL_Color secondGlowColor; // second M-Speed glow color
extern void updateRingTextures();


extern int shapeIndex;
extern int shapeIndexMaximum;

extern int ringOuterRadius;
extern int ringThickness;
extern SDL_Color ringColor;
extern SDL_Color ringOutlineColor;
extern int secondRingOuterRadius;
extern int secondRingThickness;
extern SDL_Color secondRingColor;
extern SDL_Color secondRingOutlineColor;

extern int stickR, stickG, stickB;
extern int bgR, bgG, bgB;
extern bool thickLine;

extern SDL_Color secondCrossColor; // Crosshair color
extern SDL_Color firstCrossColor;  // Markings color

extern float NorthYFloor;
extern float NorthYCeil;
extern float NorthXFloor;
extern float NorthXCeil;
extern float WestYFloor;
extern float WestYCeil;
extern float WestXFloor;
extern float WestXCeil;
extern float EastYFloor;
extern float EastYCeil;
extern float EastXFloor;
extern float EastXCeil;
extern float SouthYFloor;
extern float SouthYCeil;
extern float SouthXFloor;
extern float SouthXCeil;

extern int CornerMin;
extern int CornerMax;

// --- Helper Functions ---

// Formats a float value with the given number of decimals.
std::string formatFloat(float value, int decimals) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.*f", decimals, value);
    return std::string(buffer);
}

// Clamps integer values based on the key.
int clampIntValue(const std::string& key, int value) {
    // For booleans: only 0 or 1.
    if (key == "showCoordinatesSwitch" || key == "showMSpeedSwitch" ||
        key == "toonSwitch" || key == "dPadSwitch") {
        if (value < 0) value = 0;
        if (value > 1) value = 1;
        return value;
    }
    // For color controls (keys ending with R, G, or B).
    else if (key == "stickR" || key == "stickG" || key == "stickB" ||
        key == "joystickCircleColorR" || key == "joystickCircleColorG" || key == "joystickCircleColorB" ||
        key == "secondCrossColorR" || key == "secondCrossColorG" || key == "secondCrossColorB" ||
        key == "firstCrossColorR" || key == "firstCrossColorG" || key == "firstCrossColorB" ||
        key == "ringColorR" || key == "ringColorG" || key == "ringColorB" ||
        key == "secondRingColorR" || key == "secondRingColorG" || key == "secondRingColorB" ||
        key == "joystickGlowColorR" || key == "joystickGlowColorG" || key == "joystickGlowColorB" ||
        key == "secondGlowColorR" || key == "secondGlowColorG" || key == "secondGlowColorB" ||
        key == "bgR" || key == "bgG" || key == "bgB") {
        if (value < 0) value = 0;
        if (value > 255) value = 255;
        return value;
    }
    // For pulseAmplitude or keys containing "Floor" or "Ceil", negatives are allowed.
    else if (key == "pulseAmplitude" || key.find("Floor") != std::string::npos ||
        key.find("Ceil") != std::string::npos) {
        return value;
    }
    else {
        if (value < 0) value = 0;
        return value;
    }
}

// Clamps float values based on the key.
float clampFloatValue(const std::string& key, float value) {
    if (key == "pulseRate") {
        if (value < 0) value = 0;
        return value;
    }
    else if (key.find("Floor") != std::string::npos || key.find("Ceil") != std::string::npos) {
        return value;
    }
    else {
        if (value < 0) value = 0;
        return value;
    }
}

// --- DialogMenu Member Function Definitions ---

SDL_Color DialogMenu::getDynamicColorForField(const std::string& key) {
    if (key.find("joystickCircleColor") != std::string::npos) {
        return joystickCircleColor;
    }
    else if (key.find("secondCrossColor") != std::string::npos) {
        return secondCrossColor;
    }
    else if (key.find("firstCrossColor") != std::string::npos) {
        return firstCrossColor;
    }
    else if (key.find("ringColor") != std::string::npos && key.find("ringOutline") == std::string::npos) {
        return ringColor;
    }
    else if (key.find("secondRingColor") != std::string::npos && key.find("secondRingOutline") == std::string::npos) {
        return secondRingColor;
    }
    else if (key.find("joystickGlowColor") != std::string::npos) {
        return joystickGlowColor;
    }
    else if (key.find("secondGlowColor") != std::string::npos) {
        return secondGlowColor;
    }
    SDL_Color defaultColor = { 255, 255, 255, 255 };
    return defaultColor;
}

DialogMenu::DialogMenu()
    : active(false), selectedField(0), font(nullptr), dialogWindow(nullptr), dialogRenderer(nullptr)
{
    optionsRect = { 10, 10, 780, 420 };
    bgColor = { 50, 50, 50, 230 };
    borderColor = { 200, 200, 200, 255 };
}

DialogMenu::~DialogMenu() {
    if (dialogRenderer) {
        SDL_DestroyRenderer(dialogRenderer);
        dialogRenderer = nullptr;
    }
    if (dialogWindow) {
        SDL_DestroyWindow(dialogWindow);
        dialogWindow = nullptr;
    }
}

void DialogMenu::init(TTF_Font* f) {
    font = f;
    refreshFields();
}

void DialogMenu::setActive(bool a) {
    if (a && !active) {
        dialogWindow = SDL_CreateWindow("Settings", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_SHOWN);
        if (!dialogWindow) {
            SDL_Log("Failed to create settings window: %s", SDL_GetError());
            return;
        }
        dialogRenderer = SDL_CreateRenderer(dialogWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!dialogRenderer) {
            SDL_Log("Failed to create renderer for settings window: %s", SDL_GetError());
            SDL_DestroyWindow(dialogWindow);
            dialogWindow = nullptr;
            return;
        }
        SDL_StartTextInput();
        active = true;
    }
    else if (!a && active) {
        SDL_StopTextInput();
        if (dialogRenderer) {
            SDL_DestroyRenderer(dialogRenderer);
            dialogRenderer = nullptr;
        }
        if (dialogWindow) {
            SDL_DestroyWindow(dialogWindow);
            dialogWindow = nullptr;
        }
        active = false;
    }
}

bool DialogMenu::isActive() const {
    return active;
}

void DialogMenu::refreshFields() {
    fields.clear();
    selectedField = 0;
    // Group 1: General Options
    fields.push_back({ "showCoordinatesSwitch", "(F1) Show Coordinates", SET_BOOL, (showCoordinatesSwitch ? "1" : "0") });
    fields.push_back({ "showMSpeedSwitch", "(F2) M Speed Mode", SET_BOOL, (showMSpeedSwitch ? "1" : "0") });
    fields.push_back({ "toonSwitch", "(F3) Show / Hide Toon", SET_BOOL, (toonSwitch ? "1" : "0") });
    fields.push_back({ "dPadSwitch", "(F4) Map D-Pad to Stick", SET_BOOL, (dPadSwitch ? "1" : "0") });
    // Group 2: Joystick / Reticle Settings
    fields.push_back({ "stickR", "Joystick Color (R)", SET_INT, std::to_string(stickR) });
    fields.push_back({ "stickG", "Joystick Color (G)", SET_INT, std::to_string(stickG) });
    fields.push_back({ "stickB", "Joystick Color (B)", SET_INT, std::to_string(stickB) });
    fields.push_back({ "circleSize", "Reticle Size", SET_INT, std::to_string(circleSize) });
    fields.push_back({ "joystickCircleColorR", "Reticle Color (R)", SET_INT, std::to_string(joystickCircleColor.r) });
    fields.push_back({ "joystickCircleColorG", "Reticle Color (G)", SET_INT, std::to_string(joystickCircleColor.g) });
    fields.push_back({ "joystickCircleColorB", "Reticle Color (B)", SET_INT, std::to_string(joystickCircleColor.b) });
    // Group 3: Crosshair & Markings Colors
    fields.push_back({ "secondCrossColorR", "Crosshair Color (R)", SET_INT, std::to_string(secondCrossColor.r) });
    fields.push_back({ "secondCrossColorG", "Crosshair Color (G)", SET_INT, std::to_string(secondCrossColor.g) });
    fields.push_back({ "secondCrossColorB", "Crosshair Color (B)", SET_INT, std::to_string(secondCrossColor.b) });
    fields.push_back({ "firstCrossColorR", "Markings Color (R)", SET_INT, std::to_string(firstCrossColor.r) });
    fields.push_back({ "firstCrossColorG", "Markings Color (G)", SET_INT, std::to_string(firstCrossColor.g) });
    fields.push_back({ "firstCrossColorB", "Markings Color (B)", SET_INT, std::to_string(firstCrossColor.b) });
    // Group 4: Ring Settings
    fields.push_back({ "shapeIndex", "(F5) Ring Shape", SET_INT, std::to_string(shapeIndex) });
    fields.push_back({ "ringOuterRadius", "Large Ring Radius", SET_INT, std::to_string(ringOuterRadius) });
    fields.push_back({ "ringThickness", "Large Ring Thickness", SET_INT, std::to_string(ringThickness) });
    fields.push_back({ "ringColorR", "Large Ring Color (R)", SET_INT, std::to_string(ringColor.r) });
    fields.push_back({ "ringColorG", "Large Ring Color (G)", SET_INT, std::to_string(ringColor.g) });
    fields.push_back({ "ringColorB", "Large Ring Color (B)", SET_INT, std::to_string(ringColor.b) });
    fields.push_back({ "secondRingOuterRadius", "Small Ring Radius", SET_INT, std::to_string(secondRingOuterRadius) });
    fields.push_back({ "secondRingThickness", "Small Ring Thickness", SET_INT, std::to_string(secondRingThickness) });
    fields.push_back({ "secondRingColorR", "Small Ring Color (R)", SET_INT, std::to_string(secondRingColor.r) });
    fields.push_back({ "secondRingColorG", "Small Ring Color (G)", SET_INT, std::to_string(secondRingColor.g) });
    fields.push_back({ "secondRingColorB", "Small Ring Color (B)", SET_INT, std::to_string(secondRingColor.b) });
    // Group 5: M-Speed Glow Settings
    int dispPulse = static_cast<int>(pulseRate * 10000); // e.g., 0.0040 becomes 40.
    fields.push_back({ "pulseRate", "Pulse Rate", SET_INT, std::to_string(dispPulse) });
    fields.push_back({ "pulseAmplitude", "Pulse Amplitude", SET_INT, std::to_string(pulseAmplitude) });
    fields.push_back({ "joystickGlowColorR", "M Speed Glow 1 (R)", SET_INT, std::to_string(joystickGlowColor.r) });
    fields.push_back({ "joystickGlowColorG", "M Speed Glow 1 (G)", SET_INT, std::to_string(joystickGlowColor.g) });
    fields.push_back({ "joystickGlowColorB", "M Speed Glow 1 (B)", SET_INT, std::to_string(joystickGlowColor.b) });
    fields.push_back({ "secondGlowColorR", "M Speed Glow 2 (R)", SET_INT, std::to_string(secondGlowColor.r) });
    fields.push_back({ "secondGlowColorG", "M Speed Glow 2 (G)", SET_INT, std::to_string(secondGlowColor.g) });
    fields.push_back({ "secondGlowColorB", "M Speed Glow 2 (B)", SET_INT, std::to_string(secondGlowColor.b) });
    // Group 6: Background Color
    fields.push_back({ "bgR", "Background Color (R)", SET_INT, std::to_string(bgR) });
    fields.push_back({ "bgG", "Background Color (G)", SET_INT, std::to_string(bgG) });
    fields.push_back({ "bgB", "Background Color (B)", SET_INT, std::to_string(bgB) });
    // Group 7: Directional Thresholds & Corner Settings
    fields.push_back({ "NorthYFloor", "North - Y Floor", SET_FLOAT, formatFloat(NorthYFloor, 2) });
    fields.push_back({ "NorthYCeil", "North - Y Ceiling", SET_FLOAT, formatFloat(NorthYCeil, 2) });
    fields.push_back({ "NorthXFloor", "North - X Floor", SET_FLOAT, formatFloat(NorthXFloor, 2) });
    fields.push_back({ "NorthXCeil", "North - X Ceiling", SET_FLOAT, formatFloat(NorthXCeil, 2) });
    fields.push_back({ "WestYFloor", "West - Y Floor", SET_FLOAT, formatFloat(WestYFloor, 2) });
    fields.push_back({ "WestYCeil", "West - Y Ceiling", SET_FLOAT, formatFloat(WestYCeil, 2) });
    fields.push_back({ "WestXFloor", "West - X Floor", SET_FLOAT, formatFloat(WestXFloor, 2) });
    fields.push_back({ "WestXCeil", "West - X Ceiling", SET_FLOAT, formatFloat(WestXCeil, 2) });
    fields.push_back({ "EastYFloor", "East - Y Floor", SET_FLOAT, formatFloat(EastYFloor, 2) });
    fields.push_back({ "EastYCeil", "East - Y Ceiling", SET_FLOAT, formatFloat(EastYCeil, 2) });
    fields.push_back({ "EastXFloor", "East - X Floor", SET_FLOAT, formatFloat(EastXFloor, 2) });
    fields.push_back({ "EastXCeil", "East - X Ceiling", SET_FLOAT, formatFloat(EastXCeil, 2) });
    fields.push_back({ "SouthYFloor", "South - Y Floor", SET_FLOAT, formatFloat(SouthYFloor, 2) });
    fields.push_back({ "SouthYCeil", "South - Y Ceiling", SET_FLOAT, formatFloat(SouthYCeil, 2) });
    fields.push_back({ "SouthXFloor", "South - X Floor", SET_FLOAT, formatFloat(SouthXFloor, 2) });
    fields.push_back({ "SouthXCeil", "South - X Ceiling", SET_FLOAT, formatFloat(SouthXCeil, 2) });
    fields.push_back({ "CornerMin", "Corner Minimum", SET_INT, std::to_string(CornerMin) });
    fields.push_back({ "CornerMax", "Corner Maximum", SET_INT, std::to_string(CornerMax) });
}

void DialogMenu::handleEvent(const SDL_Event& e) {
    if (!active)
        return;
    if (e.type == SDL_WINDOWEVENT) {
        if (e.window.windowID != SDL_GetWindowID(dialogWindow))
            return;
        if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
            applyChanges();
            setActive(false);
            return;
        }
    }
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mx = e.button.x, my = e.button.y;
        SDL_Rect applyRect = { optionsRect.x, optionsRect.y + optionsRect.h + 10, optionsRect.w, 30 };
        if (mx >= applyRect.x && mx < applyRect.x + applyRect.w &&
            my >= applyRect.y && my < applyRect.y + applyRect.h) {
            applyChanges();
            setActive(false);
            return;
        }
    }
    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_UP) {
            selectedField--;
            int total = fields.size();
            if (selectedField < 0)
                selectedField = total - 1;
        }
        else if (e.key.keysym.sym == SDLK_DOWN) {
            selectedField++;
            int total = fields.size();
            if (selectedField >= total)
                selectedField = 0;
        }
        else if (e.key.keysym.sym == SDLK_LEFT || e.key.keysym.sym == SDLK_RIGHT) {
            int fieldType = fields[selectedField].type;
            if (fields[selectedField].key == "pulseRate") {
                // Work with the display value as an int.
                int currentDisplay = parseInt(fields[selectedField].valueStr, 0);
                int delta = (e.key.keysym.sym == SDLK_RIGHT) ? 1 : -1;
                currentDisplay += delta;
                // Clamp to 0-100.
                if (currentDisplay < 0)
                    currentDisplay = 0;
                if (currentDisplay > 100)
                    currentDisplay = 100;
                fields[selectedField].valueStr = std::to_string(currentDisplay);
                // Update the global pulseRate: 1 display unit = 0.0001.
                pulseRate = currentDisplay / 10000.0f;
                applyChanges();
            }
            else if (fields[selectedField].key == "shapeIndex") {
                int currentValue = parseInt(fields[selectedField].valueStr, 0);
                int delta = (e.key.keysym.sym == SDLK_RIGHT) ? 1 : -1;
                currentValue += delta;
                if (currentValue < 0) currentValue = 0;
                if (currentValue > shapeIndexMaximum) currentValue = shapeIndexMaximum;
                fields[selectedField].valueStr = std::to_string(currentValue);
                applyChanges();
            }
            else if (fieldType == SET_INT || fieldType == SET_BOOL) {
            int currentValue = parseInt(fields[selectedField].valueStr, 0);
            int delta = (e.key.keysym.sym == SDLK_RIGHT) ? 1 : -1;
            currentValue += delta;
            currentValue = clampIntValue(fields[selectedField].key, currentValue);
            fields[selectedField].valueStr = std::to_string(currentValue);
            applyChanges();
        }
            else if (fieldType == SET_FLOAT) {
                float currentValue = parseFloat(fields[selectedField].valueStr, 0.0f);
                float delta = 0.01f;
                currentValue += (e.key.keysym.sym == SDLK_RIGHT) ? delta : -delta;
                currentValue = clampFloatValue(fields[selectedField].key, currentValue);
                int decimals = 2;
                fields[selectedField].valueStr = formatFloat(currentValue, decimals);
                applyChanges();
            }
        }
        else if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_ESCAPE) {
            applyChanges();
            setActive(false);
        }
        else if (e.key.keysym.sym == SDLK_BACKSPACE) {
            if (!fields[selectedField].valueStr.empty())
                fields[selectedField].valueStr.pop_back();
        }
    }
    else if (e.type == SDL_TEXTINPUT) {
        bool valid = true;
        int fieldType = fields[selectedField].type;
        std::string currentValue = fields[selectedField].valueStr;
        const char* text = e.text.text;
        for (int i = 0; text[i] != '\0'; i++) {
            char c = text[i];
            if (fieldType == SET_INT || fieldType == SET_BOOL) {
                if (i == 0) {
                    if (!((c >= '0' && c <= '9') || c == '-'))
                        valid = false;
                }
                else {
                    if (!(c >= '0' && c <= '9'))
                        valid = false;
                }
            }
            else if (fieldType == SET_FLOAT) {
                if (i == 0) {
                    if (!((c >= '0' && c <= '9') || c == '-' || c == '.'))
                        valid = false;
                }
                else {
                    if (!(c >= '0' && c <= '9') && !(c == '.' && currentValue.find('.') == std::string::npos))
                        valid = false;
                }
            }
            if (!valid)
                break;
        }
        if (valid)
            fields[selectedField].valueStr += text;
    }
}
void DialogMenu::renderField(SDL_Renderer* renderer, const SettingField& field, const SDL_Rect& rect, bool selected)
{
    // Divide the field rectangle: 60% for the label and 40% for the value.
    int labelWidth = rect.w * 60 / 100;
    int valueWidth = rect.w - labelWidth;
    SDL_Rect labelRect = { rect.x, rect.y, labelWidth, rect.h };
    SDL_Rect valueRect = { rect.x + labelWidth, rect.y, valueWidth, rect.h };

    SDL_Color defaultLabelColor = selected ? SDL_Color{ 255,255,0,255 } : SDL_Color{ 200,200,200,255 };
    std::string labelText = field.label;

    // Look for dynamic keywords "Color" and "Glow".
    size_t posColor = labelText.find("Color");
    size_t posGlow = labelText.find("Glow");
    size_t pos = std::string::npos;
    std::string dynamicWord;
    if (posColor != std::string::npos && posGlow != std::string::npos) {
        if (posColor < posGlow) {
            pos = posColor;
            dynamicWord = "Color";
        }
        else {
            pos = posGlow;
            dynamicWord = "Glow";
        }
    }
    else if (posColor != std::string::npos) {
        pos = posColor;
        dynamicWord = "Color";
    }
    else if (posGlow != std::string::npos) {
        pos = posGlow;
        dynamicWord = "Glow";
    }

    if (pos != std::string::npos) {
        std::string leftPart = labelText.substr(0, pos);
        std::string rightPart = labelText.substr(pos + dynamicWord.size());

        SDL_Surface* leftSurface = TTF_RenderText_Solid(font, leftPart.c_str(), defaultLabelColor);
        SDL_Texture* leftTexture = leftSurface ? SDL_CreateTextureFromSurface(renderer, leftSurface) : nullptr;
        int leftW = 0, leftH = 0;
        if (leftTexture)
            SDL_QueryTexture(leftTexture, NULL, NULL, &leftW, &leftH);

        SDL_Color dynColor = getDynamicColorForField(field.key);
        SDL_Surface* dynSurface = TTF_RenderText_Solid(font, dynamicWord.c_str(), dynColor);
        SDL_Texture* dynTexture = dynSurface ? SDL_CreateTextureFromSurface(renderer, dynSurface) : nullptr;
        int dynW = 0, dynH = 0;
        if (dynTexture)
            SDL_QueryTexture(dynTexture, NULL, NULL, &dynW, &dynH);

        SDL_Surface* rightSurface = TTF_RenderText_Solid(font, rightPart.c_str(), defaultLabelColor);
        SDL_Texture* rightTexture = rightSurface ? SDL_CreateTextureFromSurface(renderer, rightSurface) : nullptr;
        int rightW = 0, rightH = 0;
        if (rightTexture)
            SDL_QueryTexture(rightTexture, NULL, NULL, &rightW, &rightH);

        int currentX = labelRect.x + 2;
        if (leftTexture) {
            SDL_Rect dst = { currentX, labelRect.y + (labelRect.h - leftH) / 2, leftW, leftH };
            SDL_RenderCopy(renderer, leftTexture, NULL, &dst);
            currentX += leftW;
        }
        if (dynTexture) {
            SDL_Rect dst = { currentX, labelRect.y + (labelRect.h - dynH) / 2, dynW, dynH };
            SDL_RenderCopy(renderer, dynTexture, NULL, &dst);
            currentX += dynW;
        }
        if (rightTexture) {
            SDL_Rect dst = { currentX, labelRect.y + (labelRect.h - rightH) / 2, rightW, rightH };
            SDL_RenderCopy(renderer, rightTexture, NULL, &dst);
        }
        if (leftSurface) SDL_FreeSurface(leftSurface);
        if (dynSurface) SDL_FreeSurface(dynSurface);
        if (rightSurface) SDL_FreeSurface(rightSurface);
        if (leftTexture) SDL_DestroyTexture(leftTexture);
        if (dynTexture) SDL_DestroyTexture(dynTexture);
        if (rightTexture) SDL_DestroyTexture(rightTexture);
    }
    else {
        SDL_Surface* labelSurface = TTF_RenderText_Solid(font, labelText.c_str(), defaultLabelColor);
        if (labelSurface) {
            SDL_Texture* labelTexture = SDL_CreateTextureFromSurface(renderer, labelSurface);
            int texW = 0, texH = 0;
            SDL_QueryTexture(labelTexture, NULL, NULL, &texW, &texH);
            SDL_Rect dstLabel = { labelRect.x + 2, labelRect.y + (labelRect.h - texH) / 2, texW, texH };
            SDL_RenderCopy(renderer, labelTexture, NULL, &dstLabel);
            SDL_FreeSurface(labelSurface);
            SDL_DestroyTexture(labelTexture);
        }
    }

    SDL_Color valueColor = selected ? SDL_Color{ 255,255,0,255 } : SDL_Color{ 255,255,255,255 };
    SDL_Surface* valueSurface = TTF_RenderText_Solid(font, field.valueStr.c_str(), valueColor);
    if (valueSurface) {
        SDL_Texture* valueTexture = SDL_CreateTextureFromSurface(renderer, valueSurface);
        int texW = 0, texH = 0;
        SDL_QueryTexture(valueTexture, NULL, NULL, &texW, &texH);
        int xPos = valueRect.x + valueRect.w - texW - 2;
        SDL_Rect dstValue = { xPos, valueRect.y + (valueRect.h - texH) / 2, texW, texH };
        SDL_RenderCopy(renderer, valueTexture, NULL, &dstValue);
        SDL_FreeSurface(valueSurface);
        SDL_DestroyTexture(valueTexture);
    }
}

void DialogMenu::render() {
    if (!active || !dialogRenderer)
        return;

    // Clear and fill background.
    SDL_SetRenderDrawColor(dialogRenderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderClear(dialogRenderer);

    // Define the main panel area.
    SDL_Rect panel = { 10, 10, 780, 420 };
    SDL_SetRenderDrawColor(dialogRenderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(dialogRenderer, &panel);
    SDL_SetRenderDrawColor(dialogRenderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderDrawRect(dialogRenderer, &panel);

    // --- Step 1: Partition fields into groups based on first 3 characters of the label ---
    std::vector<std::vector<SettingField>> groups;
    if (!fields.empty()) {
        std::vector<SettingField> currentGroup;
        currentGroup.push_back(fields[0]);
        std::string currPrefix = fields[0].label.substr(0, std::min((size_t)3, fields[0].label.size()));
        for (size_t i = 1; i < fields.size(); i++) {
            std::string prefix = fields[i].label.substr(0, std::min((size_t)3, fields[i].label.size()));
            if (prefix == currPrefix) {
                currentGroup.push_back(fields[i]);
            }
            else {
                groups.push_back(currentGroup);
                currentGroup.clear();
                currentGroup.push_back(fields[i]);
                currPrefix = prefix;
            }
        }
        if (!currentGroup.empty())
            groups.push_back(currentGroup);
    }

    // --- Step 2: Partition groups into columns, without splitting a group ---
    const int capacity = 16; // max number of controls per column
    std::vector<std::vector<std::vector<SettingField>>> colGroups; // each column is a vector of groups
    std::vector<std::vector<SettingField>> currColGroups;
    int currCount = 0;
    for (auto& grp : groups) {
        // If current column is empty or the entire group fits:
        if (currCount == 0 || (currCount + (int)grp.size() <= capacity)) {
            currColGroups.push_back(grp);
            currCount += grp.size();
        }
        else {
            // Start new column
            colGroups.push_back(currColGroups);
            currColGroups.clear();
            currColGroups.push_back(grp);
            currCount = grp.size();
        }
    }
    if (!currColGroups.empty())
        colGroups.push_back(currColGroups);

    int numColumns = colGroups.size();
    int colWidth = panel.w / numColumns;

    // --- Step 3: Render each column ---
    for (int col = 0; col < numColumns; col++) {
        int startX = panel.x + col * colWidth;
        int currentY = panel.y + 5; // initial top padding

        // For each group in this column:
        for (size_t g = 0; g < colGroups[col].size(); g++) {
            std::vector<SettingField>& group = colGroups[col][g];
            // For each field in the group:
            for (size_t f = 0; f < group.size(); f++) {
                SDL_Rect fieldRect = {
                    startX + 5,
                    currentY,
                    colWidth - 10,
                    20  // fixed field height
                };
                // Compute the global index for selection:
                int globalIndex = 0;
                for (int c = 0; c < col; c++) {
                    for (auto& grpInCol : colGroups[c])
                        globalIndex += grpInCol.size();
                }
                for (size_t k = 0; k < g; k++) {
                    globalIndex += colGroups[col][k].size();
                }
                globalIndex += f;
                bool selected = (globalIndex == selectedField);
                renderField(dialogRenderer, group[f], fieldRect, selected);
                currentY += 25; // field height (20) + vertical spacing (5)
            }
            // Draw a horizontal dividing line between groups, except after the last group in the column.
            if (g + 1 < colGroups[col].size()) {
                SDL_SetRenderDrawColor(dialogRenderer, 150, 150, 150, 255);
                SDL_RenderDrawLine(dialogRenderer, startX + 5, currentY, startX + colWidth - 5, currentY);
                currentY += 5; // extra gap after divider
            }
        }
    }

    // --- Step 4: Draw the Apply button below the panel ---
    SDL_Rect applyRect = { panel.x, panel.y + panel.h + 10, panel.w, 30 };
    SDL_SetRenderDrawColor(dialogRenderer, 100, 100, 100, 255);
    SDL_RenderFillRect(dialogRenderer, &applyRect);
    SDL_SetRenderDrawColor(dialogRenderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderDrawRect(dialogRenderer, &applyRect);
    SDL_Surface* surface = TTF_RenderText_Solid(font, "Apply", { 255,255,255,255 });
    SDL_Texture* textTex = SDL_CreateTextureFromSurface(dialogRenderer, surface);
    int textW = 0, textH = 0;
    SDL_QueryTexture(textTex, NULL, NULL, &textW, &textH);
    SDL_Rect textRect = { applyRect.x + (applyRect.w - textW) / 2, applyRect.y + (applyRect.h - textH) / 2, textW, textH };
    SDL_RenderCopy(dialogRenderer, textTex, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(textTex);

    SDL_RenderPresent(dialogRenderer);
}



bool DialogMenu::applyChanges() {
    for (const auto& field : fields) {
        if (field.key == "showCoordinatesSwitch")
            showCoordinatesSwitch = parseBool(field.valueStr, showCoordinatesSwitch);
        else if (field.key == "showMSpeedSwitch")
            showMSpeedSwitch = parseBool(field.valueStr, showMSpeedSwitch);
        else if (field.key == "toonSwitch")
            toonSwitch = parseBool(field.valueStr, toonSwitch);
        else if (field.key == "dPadSwitch")
            dPadSwitch = parseBool(field.valueStr, dPadSwitch);
        else if (field.key == "stickR")
            stickR = parseInt(field.valueStr, stickR);
        else if (field.key == "stickG")
            stickG = parseInt(field.valueStr, stickG);
        else if (field.key == "stickB")
            stickB = parseInt(field.valueStr, stickB);
        else if (field.key == "circleSize")
            circleSize = parseInt(field.valueStr, circleSize);
        else if (field.key == "joystickCircleColorR")
            joystickCircleColor.r = (Uint8)parseInt(field.valueStr, joystickCircleColor.r);
        else if (field.key == "joystickCircleColorG")
            joystickCircleColor.g = (Uint8)parseInt(field.valueStr, joystickCircleColor.g);
        else if (field.key == "joystickCircleColorB")
            joystickCircleColor.b = (Uint8)parseInt(field.valueStr, joystickCircleColor.b);
        else if (field.key == "secondCrossColorR")
            secondCrossColor.r = (Uint8)parseInt(field.valueStr, secondCrossColor.r);
        else if (field.key == "secondCrossColorG")
            secondCrossColor.g = (Uint8)parseInt(field.valueStr, secondCrossColor.g);
        else if (field.key == "secondCrossColorB")
            secondCrossColor.b = (Uint8)parseInt(field.valueStr, secondCrossColor.b);
        else if (field.key == "firstCrossColorR")
            firstCrossColor.r = (Uint8)parseInt(field.valueStr, firstCrossColor.r);
        else if (field.key == "firstCrossColorG")
            firstCrossColor.g = (Uint8)parseInt(field.valueStr, firstCrossColor.g);
        else if (field.key == "firstCrossColorB")
            firstCrossColor.b = (Uint8)parseInt(field.valueStr, firstCrossColor.b);
        else if (field.key == "shapeIndex")
            shapeIndex = parseInt(field.valueStr, shapeIndex);
        else if (field.key == "ringOuterRadius")
            ringOuterRadius = parseInt(field.valueStr, ringOuterRadius);
        else if (field.key == "ringThickness")
            ringThickness = parseInt(field.valueStr, ringThickness);
        else if (field.key == "ringColorR")
            ringColor.r = (Uint8)parseInt(field.valueStr, ringColor.r);
        else if (field.key == "ringColorG")
            ringColor.g = (Uint8)parseInt(field.valueStr, ringColor.g);
        else if (field.key == "ringColorB")
            ringColor.b = (Uint8)parseInt(field.valueStr, ringColor.b);
        else if (field.key == "secondRingOuterRadius")
            secondRingOuterRadius = parseInt(field.valueStr, secondRingOuterRadius);
        else if (field.key == "secondRingThickness")
            secondRingThickness = parseInt(field.valueStr, secondRingThickness);
        else if (field.key == "secondRingColorR")
            secondRingColor.r = (Uint8)parseInt(field.valueStr, secondRingColor.r);
        else if (field.key == "secondRingColorG")
            secondRingColor.g = (Uint8)parseInt(field.valueStr, secondRingColor.g);
        else if (field.key == "secondRingColorB")
            secondRingColor.b = (Uint8)parseInt(field.valueStr, secondRingColor.b);
        else if (field.key == "pulseRate") {
            // Convert the display value (0-100) to the float range (0.0000 to 0.0100)
            int disp = parseInt(field.valueStr, 0);
            if (disp < 0) disp = 0;
            if (disp > 100) disp = 100;
            pulseRate = disp / 10000.0f;
        }
        else if (field.key == "pulseAmplitude")
            pulseAmplitude = parseInt(field.valueStr, pulseAmplitude);
        else if (field.key == "joystickGlowColorR")
            joystickGlowColor.r = (Uint8)parseInt(field.valueStr, joystickGlowColor.r);
        else if (field.key == "joystickGlowColorG")
            joystickGlowColor.g = (Uint8)parseInt(field.valueStr, joystickGlowColor.g);
        else if (field.key == "joystickGlowColorB")
            joystickGlowColor.b = (Uint8)parseInt(field.valueStr, joystickGlowColor.b);
        else if (field.key == "secondGlowColorR")
            secondGlowColor.r = (Uint8)parseInt(field.valueStr, secondGlowColor.r);
        else if (field.key == "secondGlowColorG")
            secondGlowColor.g = (Uint8)parseInt(field.valueStr, secondGlowColor.g);
        else if (field.key == "secondGlowColorB")
            secondGlowColor.b = (Uint8)parseInt(field.valueStr, secondGlowColor.b);
        else if (field.key == "bgR")
            bgR = parseInt(field.valueStr, bgR);
        else if (field.key == "bgG")
            bgG = parseInt(field.valueStr, bgG);
        else if (field.key == "bgB")
            bgB = parseInt(field.valueStr, bgB);
        else if (field.key == "NorthYFloor")
            NorthYFloor = parseFloat(field.valueStr, NorthYFloor);
        else if (field.key == "NorthYCeil")
            NorthYCeil = parseFloat(field.valueStr, NorthYCeil);
        else if (field.key == "NorthXFloor")
            NorthXFloor = parseFloat(field.valueStr, NorthXFloor);
        else if (field.key == "NorthXCeil")
            NorthXCeil = parseFloat(field.valueStr, NorthXCeil);
        else if (field.key == "WestYFloor")
            WestYFloor = parseFloat(field.valueStr, WestYFloor);
        else if (field.key == "WestYCeil")
            WestYCeil = parseFloat(field.valueStr, WestYCeil);
        else if (field.key == "WestXFloor")
            WestXFloor = parseFloat(field.valueStr, WestXFloor);
        else if (field.key == "WestXCeil")
            WestXCeil = parseFloat(field.valueStr, WestXCeil);
        else if (field.key == "EastYFloor")
            EastYFloor = parseFloat(field.valueStr, EastYFloor);
        else if (field.key == "EastYCeil")
            EastYCeil = parseFloat(field.valueStr, EastYCeil);
        else if (field.key == "EastXFloor")
            EastXFloor = parseFloat(field.valueStr, EastXFloor);
        else if (field.key == "EastXCeil")
            EastXCeil = parseFloat(field.valueStr, EastXCeil);
        else if (field.key == "SouthYFloor")
            SouthYFloor = parseFloat(field.valueStr, SouthYFloor);
        else if (field.key == "SouthYCeil")
            SouthYCeil = parseFloat(field.valueStr, SouthYCeil);
        else if (field.key == "SouthXFloor")
            SouthXFloor = parseFloat(field.valueStr, SouthXFloor);
        else if (field.key == "SouthXCeil")
            SouthXCeil = parseFloat(field.valueStr, SouthXCeil);
        else if (field.key == "CornerMin")
            CornerMin = parseInt(field.valueStr, CornerMin);
        else if (field.key == "CornerMax")
            CornerMax = parseInt(field.valueStr, CornerMax);
    }
    staticOverlayDirty = true;

    // IMPORTANT: Update the ring textures in real time.
    updateRingTextures();

    return true;
}

int DialogMenu::parseInt(const std::string& s, int defaultVal) {
    try {
        return std::stoi(s);
    }
    catch (...) {
        return defaultVal;
    }
}

float DialogMenu::parseFloat(const std::string& s, float defaultVal) {
    try {
        return std::stof(s);
    }
    catch (...) {
        return defaultVal;
    }
}

bool DialogMenu::parseBool(const std::string& s, bool defaultVal) {
    try {
        int val = std::stoi(s);
        return (val != 0);
    }
    catch (...) {
        return defaultVal;
    }
}

int DialogMenu::getInt(const std::string& key, int defaultVal) {
    for (const auto& field : fields) {
        if (field.key == key)
            return parseInt(field.valueStr, defaultVal);
    }
    return defaultVal;
}

float DialogMenu::getFloat(const std::string& key, float defaultVal) {
    for (const auto& field : fields) {
        if (field.key == key)
            return parseFloat(field.valueStr, defaultVal);
    }
    return defaultVal;
}

bool DialogMenu::getBool(const std::string& key, bool defaultVal) {
    for (const auto& field : fields) {
        if (field.key == key)
            return parseBool(field.valueStr, defaultVal);
    }
    return defaultVal;
}
