#include <SDL/SDL_image.h>
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#include <SDL/SDL_hints.h>
#include <SDL/SDL_ttf.h>

#include <Windows.h>
#include <tlhelp32.h>
#include <Winuser.h>

#include <stdio.h>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>

#include "Getline.hpp"
#include "Split.hpp"
#include "DialogMenu.h"  // Include the dialog menu header

// ===== Editable Pulse/Glow Parameters =====
float pulseRate = 0.005f;   // Controls the speed of the pulse cycle (in cycles per millisecond scale)
int pulseAmplitude = 6;     // Additional pixels added to the pulsing effect.
int glowMinAlpha = 0;       // Minimum alpha value for the pulsing glow.
// ===========================================

// Global variables for folders, fonts, and messages
std::string helpPath = "./help.png";
std::string helpPath2 = "./help2.png";
std::string fontPath = "./ARLRDBD.ttf";
std::string messageToDisplay = "Init";
std::string messageToDisplay2 = "Init";
std::string messageToDisplay3 = "  M  ";

// Global configuration for the joystick indicator (when not in M‑speed mode)
SDL_Color joystickCircleColor = { 255, 255, 255, 255 }; // White
SDL_Color joystickGlowColor = { 255, 0, 0, 255 }; // First glow color (editable in settings)
SDL_Color secondGlowColor = { 177, 223, 255, 255 };   // Second glow color (change as desired)

// Global containers for preloaded animation frames
std::vector<SDL_Texture*> toonFrames;    // for normal mode
std::vector<SDL_Texture*> toonMFrames;   // for mSpeed mode

// Global ring settings 
// When M‑speed is not active, these rings are drawn a default color
int ringOuterRadius = 128;   // Outer radius of the first ring.
int ringThickness = 8;       // Thickness of the first ring.
SDL_Color ringColor = { 255, 255, 255, 255 };         // when not M‑speed
SDL_Color ringOutlineColor = { 100, 100, 100, 240 };        // outline


// Second ring settings
int secondRingOuterRadius = 40;   // Outer radius of the second ring.
int secondRingThickness = 4;      // Thickness of the second ring.
SDL_Color secondRingColor = { 255, 255, 255, 255 };   // when not M‑speed
SDL_Color secondRingOutlineColor = { 100, 100, 100, 240 };  // outline

// First crosshair (a + sign with an X overlay, i.e. 8 lines)
SDL_Color firstCrossColor = { 255, 0, 0, 255 };  // red
int firstCrossThickness = 3;
int firstCrossLength = 54;  // a few pixels larger than the second ring's radius

// Global center coordinates.
int centerX = 288;
int centerY = 144;

// Second crosshair (a simple + sign)
SDL_Color secondCrossColor = { 255, 255, 255, 255 }; // white
int secondCrossThickness = 4;
int secondCrossLength = 12;  // adjust as desired

//Global for shape
int shapeIndex = 0;
int shapeIndexMaximum = 5;

//Button Set Globals
int buttonSetLayoutIndex = 0;
int blockOffsetX = 0;  // Default: no offset.
int blockOffsetY = 0;  // You can leave Y as 0 if you only need horizontal shifts.

float effectiveRadius = 0;

// Global SDL objects and textures
SDL_Window* sdlWindow = nullptr;
SDL_Renderer* sdlRenderer = nullptr;

SDL_Texture* imgBaseToon = nullptr;
SDL_Texture* imgBaseToonM = nullptr;
SDL_Texture* imgA = nullptr;
SDL_Texture* imgB = nullptr;
SDL_Texture* imgX = nullptr;
SDL_Texture* imgY = nullptr;
SDL_Texture* imgL = nullptr;
SDL_Texture* imgR = nullptr;
SDL_Texture* imgS = nullptr;
SDL_Texture* imgLT = nullptr;
SDL_Texture* imgRT = nullptr;
SDL_Texture* imgUp = nullptr;
SDL_Texture* imgRight = nullptr;
SDL_Texture* imgDown = nullptr;
SDL_Texture* imgLeft = nullptr;
SDL_Texture* imgSS = nullptr;
SDL_Texture* imgMSM = nullptr;
SDL_Texture* imgHelp1 = nullptr;
SDL_Texture* imgHelp2 = nullptr;
SDL_Texture* texMessage = nullptr;
SDL_Texture* texMessage2 = nullptr;
SDL_Texture* texMessage3 = nullptr;
SDL_Texture* ringTexture = nullptr;
SDL_Texture* ringLayerTexture = nullptr;
SDL_Texture* ringInnerLayerTexture = nullptr;
SDL_Texture* ringLayerTextureWhite = nullptr;
SDL_Texture* ringInnerLayerTextureWhite = nullptr;



// Off-screen texture for static shapes
SDL_Texture* staticOverlayTexture = nullptr;
bool staticOverlayDirty = true; // Mark as dirty initially

bool staticButtonOverlaySwitch = 0;
int staticButtonOpacity = 27;


SDL_Surface* surfaceMessage = nullptr;
SDL_Surface* surfaceMessage2 = nullptr;
SDL_Surface* surfaceMessage3 = nullptr;

//speed check values
// Define directional thresholds with your desired default values
float NorthYFloor = 37.0f;
float NorthYCeil = 41.0f;
float NorthXFloor = -37.0f;
float NorthXCeil = 39.9f;
float WestYFloor = -36.0f;
float WestYCeil = 38.81f;
float WestXFloor = -41.0f;
float WestXCeil = -37.0f;
float EastYFloor = -36.0f;
float EastYCeil = 38.32f;
float EastXFloor = 37.0f;
float EastXCeil = 41.0f;
float SouthYFloor = -41.0f;
float SouthYCeil = -38.0f;
float SouthXFloor = -37.0f; 
float SouthXCeil = 37.0f;   

int CornerMin = -37;
int CornerMax = 37;

int CornerMinN = 0;
int CornerMaxN = 0;

// Global smoothed joystick values
float smoothedJoyX = 0.0f;
float smoothedJoyY = 0.0f;

// Global configuration for the small circle (joystick indicator)
int circleSize = 4;

// Rectangle for drawing toon images (to preserve original position/size)
SDL_Rect recBase = { 152, 8, 272, 272 };

SDL_Rect recX = { 12, 144, 64, 64 };
SDL_Rect recA = { 84, 144, 64, 64 };
SDL_Rect recB = { 84, 72, 64, 64 };
SDL_Rect recY = { 12, 72, 64, 64 };
SDL_Rect recL = { 16, 32, 48, 32 };
SDL_Rect recR = { 96, 32, 48, 32 };
SDL_Rect recLT = { 16, 216, 48, 32 };
SDL_Rect recRT = { 96, 216, 48, 32 };
SDL_Rect recS = { 376, 10, 32, 32 };
SDL_Rect recLeft = { 16, 252, 32, 32 };
SDL_Rect recDown = { 82, 252, 32, 32 };
SDL_Rect recUp = { 49, 252, 32, 32 };
SDL_Rect recRight = { 115, 252, 32, 32 };
SDL_Rect recSS = { 344, 268, 48, 32 };
SDL_Rect recMSM = { 388, 268, 32, 32 };
SDL_Rect recFull = { 0, 0, 420, 300 };
SDL_Rect recMessage = { 184, 280, 0, 20 };
SDL_Rect recMessage2 = { 268, 280, 0, 20 };
SDL_Rect recMessage3 = { 380, 278, 34, 20 };
SDL_Color ringFill = { 163, 163, 163, 255 };

SDL_Color White = { 255, 255, 255, 255 };
TTF_Font* Sans = nullptr;
TTF_Font* SansMenu = nullptr;

// Controller state variables
int JoyX = 0;
int JoyY = 0;
int A = 0;
int B = 0;
int X = 0;
int Y = 0;
int L = 0;
int R = 0;
int S = 0;
int LT = 0;
int RT = 0;
int pUp = 0;
int pDown = 0;
int pRight = 0;
int pLeft = 0;
bool tlFlag = false;
bool trFlag = false;

int animationIndexNumber = 0;
int indexLast = 0;
int folderIndexLast = 0;
bool mSpeed = false;
bool showMSpeedSwitch = false;
int folderIndex = 0;
int timerValue = 0;
bool controllerIsConnected = false;

bool showCoordinatesSwitch = false;
bool stickSensitivitySwitch = false;
bool toonSwitch = true;
bool dPadSwitch = false;
// (Removed old showHelpSwitch usage – h now opens the settings dialog)

// Additional configuration variables
bool thickLine = true;
int stickR = 0, stickG = 0, stickB = 0; // Stick line color
int bgR = 0, bgG = 0, bgB = 0;          // Background color

float thirdCrossAngleOffset = 65.0f;  // Change this to adjust the rotation.

std::vector<std::string> buttonSetFolders;
int buttonSetIndex = 0;
std::vector<std::string> toonSetFolders;
int toonSetIndex = 0;


// Forward declarations
void pollAndUpdateGameController();
std::vector<std::string> getButtonSetFolders(const std::string& directory);
std::vector<std::string> getToonSetFolders(const std::string& directory);
void loadImages();
void reloadImages();
void UpdateAnimation();
void cleanUp();
void saveIndex();
void loadSettings();
void saveSettings();
void setIcon();

static void updateText(float JoyXCoord, float JoyYCoord)
{
    char bufferX[32], bufferY[32];
    snprintf(bufferX, sizeof(bufferX), "%.2f", JoyXCoord);
    snprintf(bufferY, sizeof(bufferY), "%.2f", JoyYCoord);
    messageToDisplay = "X:  " + std::string(bufferX);
    messageToDisplay2 = "    Y:   " + std::string(bufferY);
}

// --- Drawing helper functions ---
void DrawFilledCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int dy = -radius; dy <= radius; dy++)
    {
        int dx = (int)std::sqrt((float)(radius * radius - dy * dy));
        SDL_RenderDrawLine(renderer, centerX - dx, centerY + dy, centerX + dx, centerY + dy);
    }
}

void DrawThickLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int thickness)
{
    if (thickness <= 1)
    {
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
    else
    {
        float angle = std::atan2((float)(y2 - y1), (float)(x2 - x1));
        int half = thickness / 2;
        // Draw multiple lines offset perpendicular to the line's angle
        for (int offset = -half; offset <= half; offset++)
        {
            int ox = (int)(-offset * std::sin(angle));
            int oy = (int)(offset * std::cos(angle));
            SDL_RenderDrawLine(renderer, x1 + ox, y1 + oy, x2 + ox, y2 + oy);
        }
    }
}


// Fills a convex quadrilateral defined by 4 SDL_Points.
void DrawFilledQuad(SDL_Renderer* renderer, const SDL_Point pts[4], SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    // Find the vertical bounds of the polygon.
    int minY = pts[0].y, maxY = pts[0].y;
    for (int i = 1; i < 4; i++) {
        minY = std::min(minY, pts[i].y);
        maxY = std::max(maxY, pts[i].y);
    }
    // For each scanline, compute intersections with the polygon edges.
    for (int y = minY; y <= maxY; y++) {
        std::vector<float> intersections;
        for (int i = 0; i < 4; i++) {
            int j = (i + 1) % 4;
            int y1 = pts[i].y;
            int y2 = pts[j].y;
            // Check if the scanline intersects the edge.
            if ((y >= y1 && y <= y2) || (y >= y2 && y <= y1)) {
                if (y1 == y2) {
                    // Horizontal edge: take both endpoints.
                    intersections.push_back((float)pts[i].x);
                    intersections.push_back((float)pts[j].x);
                }
                else {
                    float t = (float)(y - y1) / (float)(y2 - y1);
                    float x = pts[i].x + t * (pts[j].x - pts[i].x);
                    intersections.push_back(x);
                }
            }
        }
        if (intersections.size() >= 2) {
            std::sort(intersections.begin(), intersections.end());
            int xStart = (int)ceil(intersections.front());
            int xEnd = (int)floor(intersections.back());
            SDL_RenderDrawLine(renderer, xStart, y, xEnd, y);
        }
    }
}

// Draws a thick line as a filled quadrilateral between (x1,y1) and (x2,y2)
void DrawThickLineRect(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int thickness, SDL_Color color)
{
    // Compute the direction vector.
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len == 0)
        return;

    // Compute the normalized perpendicular vector.
    float pdx = -dy / len;
    float pdy = dx / len;
    float halfThickness = thickness / 1.5f;

    // Calculate the four corners of the thick line rectangle.
    SDL_Point quad[4];
    quad[0].x = (int)round(x1 + pdx * halfThickness);
    quad[0].y = (int)round(y1 + pdy * halfThickness);
    quad[1].x = (int)round(x2 + pdx * halfThickness);
    quad[1].y = (int)round(y2 + pdy * halfThickness);
    quad[2].x = (int)round(x2 - pdx * halfThickness);
    quad[2].y = (int)round(y2 - pdy * halfThickness);
    quad[3].x = (int)round(x1 - pdx * halfThickness);
    quad[3].y = (int)round(y1 - pdy * halfThickness);

    // Fill the quadrilateral.
    DrawFilledQuad(renderer, quad, color);
}

void DrawOctagon(SDL_Renderer* renderer, int centerX, int centerY, int outerRadius, int thickness, SDL_Color color)
{
    const int numSides = 8;
    SDL_Point outerVertices[numSides];
    SDL_Point innerOutlineVertices[numSides];
    SDL_Point outerOutlineVertices[numSides];

    // Compute outer vertices (center line of the thick edge)
    for (int i = 0; i < numSides; i++) {
        float angle = i * (2.0f * M_PI / numSides);
        outerVertices[i].x = centerX + (int)round(outerRadius * cos(angle));
        outerVertices[i].y = centerY + (int)round(outerRadius * sin(angle));
    }
    // Compute inner outline vertices (for the inner border), at outerRadius - 1
    int innerOutlineRadius = outerRadius - 1;
    for (int i = 0; i < numSides; i++) {
        float angle = i * (2.0f * M_PI / numSides);
        innerOutlineVertices[i].x = centerX + (int)round(innerOutlineRadius * cos(angle));
        innerOutlineVertices[i].y = centerY + (int)round(innerOutlineRadius * sin(angle));
    }
    // Compute outer outline vertices (offset outward by half the thickness)
    float offset = thickness / 2.0f + 2.0f;
    for (int i = 0; i < numSides; i++) {
        float angle = i * (2.0f * M_PI / numSides);
        outerOutlineVertices[i].x = centerX + (int)round(outerRadius * cos(angle) + offset * cos(angle));
        outerOutlineVertices[i].y = centerY + (int)round(outerRadius * sin(angle) + offset * sin(angle));
    }

    // --- Step 1: Draw the inner outline (the inner border) ---
    SDL_SetRenderDrawColor(renderer, ringOutlineColor.r, ringOutlineColor.g, ringOutlineColor.b, ringOutlineColor.a);
    for (int i = 0; i < numSides; i++) {
        int j = (i + 1) % numSides;
        SDL_RenderDrawLine(renderer,
            innerOutlineVertices[i].x, innerOutlineVertices[i].y,
            innerOutlineVertices[j].x, innerOutlineVertices[j].y);
    }

    // --- Step 2: Draw the main octagon with thick edges in the given color ---
    for (int i = 0; i < numSides; i++) {
        int j = (i + 1) % numSides;
        DrawThickLineRect(renderer, outerVertices[i].x, outerVertices[i].y,
            outerVertices[j].x, outerVertices[j].y,
            thickness, color);
    }
    // Draw rounded joins at the outer vertices.
    int joinRadius = thickness / 2;
    for (int i = 0; i < numSides; i++) {
        DrawFilledCircle(renderer, outerVertices[i].x, outerVertices[i].y, joinRadius, color);
    }

    // --- Step 3: Draw the outer outline along the outer edge ---
    SDL_SetRenderDrawColor(renderer, ringOutlineColor.r, ringOutlineColor.g, ringOutlineColor.b, ringOutlineColor.a);
    for (int i = 0; i < numSides; i++) {
        int j = (i + 1) % numSides;
        SDL_RenderDrawLine(renderer,
            outerOutlineVertices[i].x, outerOutlineVertices[i].y,
            outerOutlineVertices[j].x, outerOutlineVertices[j].y);
    }
}



void DrawCircleOutline(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    int x = radius, y = 0;
    int decision = 1 - x;
    while (y <= x)
    {
        SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
        y++;
        if (decision <= 0)
            decision += 2 * y + 1;
        else
        {
            x--;
            decision += 2 * (y - x) + 1;
        }
    }
}

void DrawRing(SDL_Renderer* renderer, int centerX, int centerY, int outerRadius, int thickness, SDL_Color fillColor, SDL_Color outlineColor)
{
    int innerRadius = outerRadius - thickness - 1;
    

    for (int dy = -outerRadius; dy <= outerRadius; dy++)
    {
        int dxOuter = (int)std::sqrt((float)(outerRadius * outerRadius - dy * dy));
        int dxInner = 0;
        if (std::abs(dy) < innerRadius)
            dxInner = (int)std::sqrt((float)(innerRadius * innerRadius - dy * dy));
        if (dxOuter > dxInner)
        {
            SDL_SetRenderDrawColor(renderer, outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
            SDL_RenderDrawLine(renderer, centerX - dxOuter, centerY + dy, centerX - dxInner, centerY + dy);
            SDL_RenderDrawLine(renderer, centerX + dxInner, centerY + dy, centerX + dxOuter, centerY + dy);
        }
    }

    innerRadius = innerRadius + 1;
    for (int dy = -outerRadius; dy <= outerRadius; dy++)
    {
        int dxOuter = (int)std::sqrt((float)(outerRadius * outerRadius - dy * dy));
        int dxInner = 0;
        if (std::abs(dy) < innerRadius)
            dxInner = (int)std::sqrt((float)(innerRadius * innerRadius - dy * dy));
        if (dxOuter > dxInner)
        {
            SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, fillColor.a);
            SDL_RenderDrawLine(renderer, centerX - dxOuter, centerY + dy, centerX - dxInner, centerY + dy);
            SDL_RenderDrawLine(renderer, centerX + dxInner, centerY + dy, centerX + dxOuter, centerY + dy);
        }
    }
    DrawCircleOutline(renderer, centerX, centerY, outerRadius, outlineColor);
}



// Helper: Draws one half of the crosshair line in a given direction,
// clipping so that only the segment outside the outer ring is drawn.
void DrawThirdCrosshairSegment(SDL_Renderer* renderer, int centerX, int centerY,
    int crossLength, int outerRingRadius,
    int thickness, SDL_Color color, float angle)
{
    // Compute unit vector for this angle.
    float cosA = cos(angle);
    float sinA = sin(angle);

    // If the crossLength doesn't exceed the outer ring, nothing to draw.
    if (crossLength <= outerRingRadius)
        return;

    // Compute the intersection point on the circle (outer ring boundary).
    int ix = centerX + (int)round(outerRingRadius * cosA);
    int iy = centerY + (int)round(outerRingRadius * sinA);

    // Compute the endpoint of the crosshair in this direction.
    int ex = centerX + (int)round(crossLength * cosA);
    int ey = centerY + (int)round(crossLength * sinA);

    // Draw a thick line from the intersection to the endpoint.
    DrawThickLineRect(renderer, ix, iy, ex, ey, thickness, color);
}

// Draws the third crosshair ("X" pattern) over the outer ring.
// It draws the portions of two mirrored rays (at +angleOffset and -angleOffset)
// that lie within the annulus defined by ringOuterRadius (outer edge)
// and ringOuterRadius - ringThickness (inner edge).
// The crosshair length is fixed to ringOuterRadius.
void DrawThirdCrosshairAnnulusMirrored(SDL_Renderer* renderer, int centerX, int centerY,
    int ringOuterRadius, int ringThickness,
    int thickness, SDL_Color color,
    float angleOffsetDegrees)
{
    // Convert the provided angle to radians.
    float theta = angleOffsetDegrees * (M_PI / 180.0f);
    // Define the inner and outer radii of the annulus.
    float innerRadius = ringOuterRadius - ringThickness;
    float outerRadius = ringOuterRadius; // crossLength equals the outer ring radius.

    // Helper lambda: Draw the ray segment for a given angle.
    auto drawRaySegment = [&](float angle) {
        // Compute the starting point (at the inner boundary) and the end point (at the outer boundary)
        int x_start = centerX + (int)round(innerRadius * cos(angle));
        int y_start = centerY + (int)round(innerRadius * sin(angle));
        int x_end = centerX + (int)round(outerRadius * cos(angle));
        int y_end = centerY + (int)round(outerRadius * sin(angle));
        DrawThickLineRect(renderer, x_start, y_start, x_end, y_end, thickness, color);

        // Also draw the ray in the opposite direction (mirroring through the center).
        x_start = centerX - (int)round(innerRadius * cos(angle));
        y_start = centerY - (int)round(innerRadius * sin(angle));
        x_end = centerX - (int)round(outerRadius * cos(angle));
        y_end = centerY - (int)round(outerRadius * sin(angle));
        DrawThickLineRect(renderer, x_start, y_start, x_end, y_end, thickness, color);
    };

    // Draw the two mirrored rays: one at +theta and one at -theta.
    drawRaySegment(theta);
    drawRaySegment(-theta);
}
SDL_Texture* createGlowTexture(int baseRadius, int extraRadius, SDL_Color baseColor) {
    int glowDiameter = (baseRadius + extraRadius) * 2;
    SDL_Texture* glowTex = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, glowDiameter, glowDiameter);
    SDL_SetTextureBlendMode(glowTex, SDL_BLENDMODE_BLEND);

    // Render to the glow texture
    SDL_SetRenderTarget(sdlRenderer, glowTex);
    SDL_SetRenderDrawBlendMode(sdlRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 0);
    SDL_RenderClear(sdlRenderer);

    // Create a temporary surface for the radial gradient
    SDL_Surface* tempSurface = SDL_CreateRGBSurfaceWithFormat(0, glowDiameter, glowDiameter, 32, SDL_PIXELFORMAT_RGBA32);
    Uint32* pixels = (Uint32*)tempSurface->pixels;
    int cx = glowDiameter / 2;
    int cy = glowDiameter / 2;
    for (int y = 0; y < glowDiameter; y++) {
        for (int x = 0; x < glowDiameter; x++) {
            int dx = x - cx;
            int dy = y - cy;
            float dist = std::sqrt(dx * dx + dy * dy);
            Uint8 alpha = (dist <= baseRadius) ? 255 :
                (dist < baseRadius + extraRadius) ? (Uint8)(255 - 255 * (dist - baseRadius) / extraRadius) : 0;
            Uint32 pixel = SDL_MapRGBA(tempSurface->format, baseColor.r, baseColor.g, baseColor.b, alpha);
            pixels[y * glowDiameter + x] = pixel;
        }
    }
    SDL_Texture* finalGlowTex = SDL_CreateTextureFromSurface(sdlRenderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    SDL_SetRenderTarget(sdlRenderer, nullptr);
    return finalGlowTex;
}
SDL_Texture* createRingTexture(int centerX, int centerY, int outerRadius, int thickness, SDL_Color fillColor, SDL_Color outlineColor) {
    // Calculate texture dimensions; we add a little extra padding.
    int texSize = (outerRadius + thickness) * 2;
    SDL_Texture* tex = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, texSize, texSize);
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

    // Set the texture as the current render target.
    SDL_SetRenderTarget(sdlRenderer, tex);
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 0);
    SDL_RenderClear(sdlRenderer);

    // Draw the ring centered in the texture.
    // We shift the center to (texSize/2, texSize/2)
    DrawRing(sdlRenderer, texSize / 2, texSize / 2, outerRadius, thickness, fillColor, outlineColor);

    // Reset render target to the default.
    SDL_SetRenderTarget(sdlRenderer, nullptr);
    return tex;
}
SDL_Texture* createOctagonTexture(int centerX, int centerY, int outerRadius, int thickness, SDL_Color color) {
    // In your older code you subtracted 4 from the outer radius and thickness.
    int adjustedRadius = outerRadius;  // Pass already adjusted values (e.g. ringOuterRadius - 4)
    int adjustedThickness = thickness; // Similarly, pass thickness already adjusted (e.g. ringThickness - 4)
    // Determine a texture size that will fully contain the octagon.
    int texSize = (adjustedRadius + adjustedThickness + 4) * 2;
    SDL_Texture* tex = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET, texSize, texSize);
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

    // Set the texture as the current render target and clear it.
    SDL_SetRenderTarget(sdlRenderer, tex);
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 0);
    SDL_RenderClear(sdlRenderer);

    // Draw the octagon centered in the texture.
    DrawOctagon(sdlRenderer, texSize / 2, texSize / 2, adjustedRadius, adjustedThickness, color);

    // Reset the render target back to the default.
    SDL_SetRenderTarget(sdlRenderer, nullptr);
    return tex;
}
void updateRingTextures() {
    // Destroy any existing textures.
    if (ringTexture != nullptr) {
        SDL_DestroyTexture(ringTexture);
        ringTexture = nullptr;
    }
    if (ringLayerTexture != nullptr) {
        SDL_DestroyTexture(ringLayerTexture);
        ringLayerTexture = nullptr;
    }
    if (ringInnerLayerTexture != nullptr) {
        SDL_DestroyTexture(ringInnerLayerTexture);
        ringInnerLayerTexture = nullptr;
    }
    if (ringLayerTextureWhite != nullptr) {
        SDL_DestroyTexture(ringLayerTextureWhite);
        ringLayerTextureWhite = nullptr;
    }
    if (ringInnerLayerTextureWhite != nullptr) {
        SDL_DestroyTexture(ringInnerLayerTextureWhite);
        ringInnerLayerTextureWhite = nullptr;
    }

    int centerX = 288 + blockOffsetX;
    int centerY = 144 + blockOffsetY;

    if (shapeIndex == 0 || shapeIndex == 1 || shapeIndex == 4 || shapeIndex == 5) {
        // Circular case (including our new modes)
        ringTexture = createRingTexture(centerX, centerY, ringOuterRadius, ringThickness, ringColor, ringOutlineColor);
        ringLayerTexture = createRingTexture(centerX, centerY, ringOuterRadius, ringThickness, ringColor, ringOutlineColor);
        ringInnerLayerTexture = createRingTexture(centerX, centerY, secondRingOuterRadius, secondRingThickness, secondRingColor, secondRingOutlineColor);
        // Create the dedicated white texture for the outer ring pulsing effect.
        SDL_Color white = { 255, 255, 255, 255 };
        ringLayerTextureWhite = createRingTexture(centerX, centerY, ringOuterRadius, ringThickness, white, white);
        // *** NEW: Create a white texture for the inner ring pulsing effect ***
        ringInnerLayerTextureWhite = createRingTexture(centerX, centerY, secondRingOuterRadius, secondRingThickness, white, white);
    }
    else if (shapeIndex == 2) {
        // Octagon case remains unchanged for the regular textures.
        ringTexture = createOctagonTexture(centerX, centerY, ringOuterRadius - 4, ringThickness - 4, ringColor);
        ringLayerTexture = createOctagonTexture(centerX, centerY, ringOuterRadius - 4, ringThickness - 4, ringColor);
        ringInnerLayerTexture = createOctagonTexture(centerX, centerY, secondRingOuterRadius - 4, secondRingThickness - 1, secondRingColor);

        // *** NEW: Create white textures for pulsing the octagon ***
        SDL_Color white = { 255, 255, 255, 255 };
        ringLayerTextureWhite = createOctagonTexture(centerX, centerY, ringOuterRadius - 4, ringThickness - 4, white);
        ringInnerLayerTextureWhite = createOctagonTexture(centerX, centerY, secondRingOuterRadius - 4, secondRingThickness - 1, white);
    }
}
void updateButtonSetLayout() {
    // Layout 0: default positions (existing layout)
    if (buttonSetLayoutIndex == 0) {
        recA = { 84, 144, 64, 64 };
        recB = { 84, 72, 64, 64 };
        recX = { 12, 144, 64, 64 };
        recY = { 12, 72, 64, 64 };
        recL = { 18, 32, 48, 32 };
        recR = { 94, 32, 48, 32 };
        recLT = { 18, 216, 48, 32 };
        recRT = { 94, 216, 48, 32 };
        recUp = { 49, 252, 32, 32 };
        recDown = { 82, 252, 32, 32 };
        recLeft = { 16, 252, 32, 32 };
        recRight = { 115, 252, 32, 32 };
        // recS (the start button) remains unchanged:
        // recS = {376, 10, 32, 32};
    }
    // Layout 1: render the button set to the right of the ring (and start button)
    else if (buttonSetLayoutIndex == 1) {
        // For this layout, we manually assign positions that place the buttons
        // on the right side of the ring centered at (288,144). Adjust these values
        // as needed for your design.
        recX = { 276, 144, 64, 64 };
        recY = { 276, 72, 64, 64 };
        recA = { 348, 144, 64, 64 };
        recB = { 348, 72, 64, 64 };
        recR = { 358, 32, 48, 32 };
        recL = { 282, 32, 48, 32 };
        recRT = { 358, 216, 48, 32 };
        recLT = { 282, 216, 48, 32 };
        recDown = { 343, 252, 32, 32 };
        recUp = { 310, 252, 32, 32 };
        recRight = { 376, 252, 32, 32 };
        recLeft = { 277, 252, 32, 32 };
        // Again, recS remains unchanged.
    }
    // Layout 2: ring and start button remain in the center.
    // The left group contains LB, Y, X, LT, Dpad Left, and Dpad Down.
    // The right group contains the remaining buttons.
    else if (buttonSetLayoutIndex == 2) {
        // Left group:
        recX = { 12, 144, 64, 64 };
        recY = { 12, 72, 64, 64 };
        recL = { 18, 32, 48, 32 };
        recLT = { 18, 216, 48, 32 };
        recUp = { 49, 252, 32, 32 };
        recLeft = { 16, 252, 32, 32 };


        // Right group:
        recA = { 348, 144, 64, 64 };
        recB = { 348, 72, 64, 64 };
        recR = { 358, 32, 48, 32 };
        recRT = { 358, 216, 48, 32 };
        recDown = { 343, 252, 32, 32 };
        recRight = { 376, 252, 32, 32 };
        // recS still remains at {376,10,32,32}.
    }
    // Layout 3: ring and start button on the left
    // Buttons in diamond shape.
    else if (buttonSetLayoutIndex == 3) {
        recX = { 267, 98, 64, 64 };  // Left button (X)
        recY = { 311, 50, 64, 64 };  // Top button (Y)
        recA = { 311, 146, 64, 64 }; // Bottom button (A)
        recB = { 355, 98, 64, 64 };  // Right button (B)
        recR = { 360, 18, 48, 32 };
        recL = { 280, 18, 48, 32 };
        recRT = { 358, 216, 48, 32 };
        recLT = { 282, 216, 48, 32 };
        recDown = { 343, 252, 32, 32 };
        recUp = { 310, 252, 32, 32 };
        recRight = { 376, 252, 32, 32 };
        recLeft = { 277, 252, 32, 32 };
        // recS still remains at {376,10,32,32}.
    }
    // Layout 4: ring and start button on the right
    // Buttons in diamond shape.
    else if (buttonSetLayoutIndex == 4) {
        recX = { 3,  98, 64, 64 };  // Left button (X) in diamond
        recY = { 47,  50, 64, 64 };   // Top button (Y)
        recA = { 47, 146, 64, 64 };   // Bottom button (A)
        recB = { 91,  98, 64, 64 };   // Right button (B)
        recR = { 96,  18, 48, 32 };
        recL = { 16,  18, 48, 32 };
        recRT = { 94, 216, 48, 32 };
        recLT = { 18, 216, 48, 32 };
        recDown = { 79, 252, 32, 32 };
        recUp = { 46, 252, 32, 32 };
        recRight = { 112, 252, 32, 32 };
        recLeft = { 13, 252, 32, 32 };
        // recS (the start button) remains unchanged.
    }
}



std::vector<std::string> getButtonSetFolders(const std::string& directory)
{
    WIN32_FIND_DATAA findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::string full_path = directory + "\\*";
    std::vector<std::string> folders;

    hFind = FindFirstFileA(full_path.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("Error: %s is not a valid directory\n", directory.c_str());
        return folders;
    }
    do {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            std::string folderName = findData.cFileName;
            if (folderName != "." && folderName != "..")
            {
                // Check for a required button image (e.g., buttA.png)
                std::string filePath = directory + "/" + folderName + "/buttA.png";
                std::ifstream f(filePath.c_str());
                if (f.good())
                    folders.push_back(folderName);
            }
        }
    } while (FindNextFileA(hFind, &findData) != 0);

    FindClose(hFind);
    return folders;
}
std::vector<std::string> getToonSetFolders(const std::string& directory)
{
    WIN32_FIND_DATAA findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::string full_path = directory + "\\*";
    std::vector<std::string> folders;

    hFind = FindFirstFileA(full_path.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("Error: %s is not a valid directory\n", directory.c_str());
        return folders;
    }
    do {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            std::string folderName = findData.cFileName;
            if (folderName != "." && folderName != "..")
            {
                // Check for a required toon image (e.g., toon0.png)
                std::string filePath = directory + "/" + folderName + "/toon0.png";
                std::ifstream f(filePath.c_str());
                if (f.good())
                    folders.push_back(folderName);
            }
        }
    } while (FindNextFileA(hFind, &findData) != 0);

    FindClose(hFind);
    return folders;
}
void loadButtonImages()
{
    if (buttonSetFolders.empty())
        return; // Or handle error

    // Construct the full path. We assume your button set folders live in ./sets/
    std::string buttonFolder = "./sets/" + buttonSetFolders[buttonSetIndex];

    imgA = IMG_LoadTexture(sdlRenderer, (buttonFolder + "/buttA.png").c_str());
    imgB = IMG_LoadTexture(sdlRenderer, (buttonFolder + "/buttB.png").c_str());
    imgX = IMG_LoadTexture(sdlRenderer, (buttonFolder + "/buttX.png").c_str());
    imgY = IMG_LoadTexture(sdlRenderer, (buttonFolder + "/buttY.png").c_str());
    imgL = IMG_LoadTexture(sdlRenderer, (buttonFolder + "/buttL.png").c_str());
    imgR = IMG_LoadTexture(sdlRenderer, (buttonFolder + "/buttR.png").c_str());
    imgLT = IMG_LoadTexture(sdlRenderer, (buttonFolder + "/buttLT.png").c_str());
    imgRT = IMG_LoadTexture(sdlRenderer, (buttonFolder + "/buttRT.png").c_str());
    imgS = IMG_LoadTexture(sdlRenderer, (buttonFolder + "/buttS.png").c_str());
    imgUp = IMG_LoadTexture(sdlRenderer, (buttonFolder + "/buttUp.png").c_str());
    imgDown = IMG_LoadTexture(sdlRenderer, (buttonFolder + "/buttDown.png").c_str());
    imgLeft = IMG_LoadTexture(sdlRenderer, (buttonFolder + "/buttLeft.png").c_str());
    imgRight = IMG_LoadTexture(sdlRenderer, (buttonFolder + "/buttRight.png").c_str());
}
void loadToonImages()
{
    if (toonSetFolders.empty())
        return; // Optionally handle the error

    // Construct the full path for the selected toon set folder.
    std::string toonFolder = "./toons/" + toonSetFolders[toonSetIndex];

    // Before loading new images, clean up any existing textures.
    for (auto tex : toonFrames) {
        if (tex != nullptr)
            SDL_DestroyTexture(tex);
    }
    for (auto tex : toonMFrames) {
        if (tex != nullptr)
            SDL_DestroyTexture(tex);
    }
    toonFrames.clear();
    toonMFrames.clear();

    const int numFrames = 8;
    toonFrames.resize(numFrames, nullptr);
    toonMFrames.resize(numFrames, nullptr);

    for (int i = 0; i < numFrames; i++) {
        std::string indexString = std::to_string(i);
        std::string toonPath = toonFolder + "/toon" + indexString + ".png";
        std::string toonMPath = toonFolder + "/toonM" + indexString + ".png";

        toonFrames[i] = IMG_LoadTexture(sdlRenderer, toonPath.c_str());
        toonMFrames[i] = IMG_LoadTexture(sdlRenderer, toonMPath.c_str());
    }
}
void loadImages()
{
    loadButtonImages();
    loadToonImages();

    if (SDL_GetTicks() / 100 <= 10)
    {

        imgHelp1 = IMG_LoadTexture(sdlRenderer, helpPath.c_str());
        imgHelp2 = IMG_LoadTexture(sdlRenderer, helpPath2.c_str());
        Sans = TTF_OpenFont(fontPath.c_str(), 17);
        SansMenu = TTF_OpenFont(fontPath.c_str(), 12);
    }

}
void reloadImages()
{

    // Clear old textures if necessary (or call a separate function to free them)
    for (auto tex : toonFrames) {
        if (tex != nullptr)
            SDL_DestroyTexture(tex);
    }
    for (auto tex : toonMFrames) {
        if (tex != nullptr)
            SDL_DestroyTexture(tex);
    }
    toonFrames.clear();
    toonMFrames.clear();

    const int numFrames = 8;
    toonFrames.resize(numFrames, nullptr);
    toonMFrames.resize(numFrames, nullptr);

    loadButtonImages();
    loadToonImages();
}
void cleanUp()
{
    if (imgBaseToon != nullptr) { SDL_DestroyTexture(imgBaseToon); imgBaseToon = nullptr; }
    if (imgBaseToonM != nullptr) { SDL_DestroyTexture(imgBaseToonM); imgBaseToonM = nullptr; }
    if (imgA != nullptr) { SDL_DestroyTexture(imgA); imgA = nullptr; }
    if (imgB != nullptr) { SDL_DestroyTexture(imgB); imgB = nullptr; }
    if (imgX != nullptr) { SDL_DestroyTexture(imgX); imgX = nullptr; }
    if (imgY != nullptr) { SDL_DestroyTexture(imgY); imgY = nullptr; }
    if (imgL != nullptr) { SDL_DestroyTexture(imgL); imgL = nullptr; }
    if (imgR != nullptr) { SDL_DestroyTexture(imgR); imgR = nullptr; }
    if (imgLT != nullptr) { SDL_DestroyTexture(imgLT); imgLT = nullptr; }
    if (imgRT != nullptr) { SDL_DestroyTexture(imgRT); imgRT = nullptr; }
    if (imgS != nullptr) { SDL_DestroyTexture(imgS); imgS = nullptr; }
    if (imgUp != nullptr) { SDL_DestroyTexture(imgUp); imgUp = nullptr; }
    if (imgDown != nullptr) { SDL_DestroyTexture(imgDown); imgDown = nullptr; }
    if (imgLeft != nullptr) { SDL_DestroyTexture(imgLeft); imgLeft = nullptr; }
    if (imgRight != nullptr) { SDL_DestroyTexture(imgRight); imgRight = nullptr; }
    if (imgSS != nullptr) { SDL_DestroyTexture(imgSS); imgSS = nullptr; }
    if (imgMSM != nullptr) { SDL_DestroyTexture(imgMSM); imgMSM = nullptr; }
    if (imgHelp2 != nullptr) { SDL_DestroyTexture(imgHelp2); imgHelp2 = nullptr; }
    if (texMessage != nullptr) { SDL_DestroyTexture(texMessage); texMessage = nullptr; }
    if (texMessage2 != nullptr) { SDL_DestroyTexture(texMessage2); texMessage2 = nullptr; }
    if (texMessage3 != nullptr) { SDL_DestroyTexture(texMessage2); texMessage3 = nullptr; }
    if (staticOverlayTexture != nullptr) { SDL_DestroyTexture(staticOverlayTexture); staticOverlayTexture = nullptr; }

}
void loadSettings()
{
    // Set default values
    showCoordinatesSwitch = false;
    showMSpeedSwitch = false;
    toonSwitch = true;
    // Removed: stickSensitivitySwitch
    dPadSwitch = false;
    circleSize = 4;
    joystickCircleColor = { 255, 255, 255, 255 };
    pulseRate = 0.005f;
    pulseAmplitude = 6;
    glowMinAlpha = 0;
    joystickGlowColor = { 255, 0, 0, 255 };
    thickLine = true;
    stickR = 0; stickG = 0; stickB = 0;
    bgR = 0; bgG = 0; bgB = 0;
    // Outer ring defaults
    ringOuterRadius = 128;
    ringThickness = 8;
    ringColor = { 255, 255, 255, 255 };
    ringOutlineColor = { 0, 0, 0, 255 };
    // Second ring defaults
    secondRingOuterRadius = 40;
    secondRingThickness = 4;
    secondRingColor = { 255, 255, 255, 255 };
    secondRingOutlineColor = { 0, 0, 0, 255 };

    // New settings defaults for directional thresholds
    NorthYFloor = 37;
    NorthYCeil = 41;
    NorthXFloor = -37;
    NorthXCeil = 39.9f;
    WestYCeil = 38.81f;
    WestYFloor = -36;
    WestXFloor = -41;
    WestXCeil = -37;
    EastYCeil = 38.32f;
    EastYFloor = -36;
    EastXFloor = 37;
    EastXCeil = 41;
    SouthYCeil = -38;
    SouthYFloor = -41;
    SouthXFloor = -37;
    SouthXCeil = 37;

    // New color and shape settings defaults
    secondCrossColor = { 255, 255, 255, 255 };
    firstCrossColor = { 255, 0, 0, 255 };
    secondGlowColor = { 177, 223, 255, 255 };
    shapeIndex = 0;

    // New corner settings defaults
    CornerMax = 37;
    CornerMin = -37;

    std::vector<std::string> lines = readFileLines("settings.ini");
    for (auto& line : lines)
    {
        std::vector<std::string> kv = split(line, '=');
        if (kv.size() != 2)
            continue;
        std::string key = kv[0];
        std::string value = kv[1];
        if (key == "showCoordinatesSwitch")
            showCoordinatesSwitch = (std::stoi(value) == 1);
        else if (key == "showMSpeedSwitch")
            showMSpeedSwitch = (std::stoi(value) == 1);
        else if (key == "toonSwitch")
            toonSwitch = (std::stoi(value) == 1);
        // Removed stickSensitivitySwitch
        else if (key == "dPadSwitch")
            dPadSwitch = (std::stoi(value) == 1);
        else if (key == "circleSize")
            circleSize = std::stoi(value);
        else if (key == "joystickCircleColorR")
            joystickCircleColor.r = (Uint8)std::stoi(value);
        else if (key == "joystickCircleColorG")
            joystickCircleColor.g = (Uint8)std::stoi(value);
        else if (key == "joystickCircleColorB")
            joystickCircleColor.b = (Uint8)std::stoi(value);
        else if (key == "pulseRate")
            pulseRate = std::stof(value);
        else if (key == "pulseAmplitude")
            pulseAmplitude = std::stoi(value);
        else if (key == "glowMinAlpha")
            glowMinAlpha = std::stoi(value);
        else if (key == "joystickGlowColorR")
            joystickGlowColor.r = (Uint8)std::stoi(value);
        else if (key == "joystickGlowColorG")
            joystickGlowColor.g = (Uint8)std::stoi(value);
        else if (key == "joystickGlowColorB")
            joystickGlowColor.b = (Uint8)std::stoi(value);
        else if (key == "thickLine")
            thickLine = (std::stoi(value) == 1);
        else if (key == "stickR")
            stickR = std::stoi(value);
        else if (key == "stickG")
            stickG = std::stoi(value);
        else if (key == "stickB")
            stickB = std::stoi(value);
        else if (key == "bgR")
            bgR = std::stoi(value);
        else if (key == "bgG")
            bgG = std::stoi(value);
        else if (key == "bgB")
            bgB = std::stoi(value);
        // Outer ring settings
        else if (key == "ringOuterRadius")
            ringOuterRadius = std::stoi(value);
        else if (key == "ringThickness")
            ringThickness = std::stoi(value);
        else if (key == "ringColorR")
            ringColor.r = (Uint8)std::stoi(value);
        else if (key == "ringColorG")
            ringColor.g = (Uint8)std::stoi(value);
        else if (key == "ringColorB")
            ringColor.b = (Uint8)std::stoi(value);
        else if (key == "ringOutlineColorR")
            ringOutlineColor.r = (Uint8)std::stoi(value);
        else if (key == "ringOutlineColorG")
            ringOutlineColor.g = (Uint8)std::stoi(value);
        else if (key == "ringOutlineColorB")
            ringOutlineColor.b = (Uint8)std::stoi(value);
        // Second ring settings
        else if (key == "secondRingOuterRadius")
            secondRingOuterRadius = std::stoi(value);
        else if (key == "secondRingThickness")
            secondRingThickness = std::stoi(value);
        else if (key == "secondRingColorR")
            secondRingColor.r = (Uint8)std::stoi(value);
        else if (key == "secondRingColorG")
            secondRingColor.g = (Uint8)std::stoi(value);
        else if (key == "secondRingColorB")
            secondRingColor.b = (Uint8)std::stoi(value);
        else if (key == "secondRingOutlineColorR")
            secondRingOutlineColor.r = (Uint8)std::stoi(value);
        else if (key == "secondRingOutlineColorG")
            secondRingOutlineColor.g = (Uint8)std::stoi(value);
        else if (key == "secondRingOutlineColorB")
            secondRingOutlineColor.b = (Uint8)std::stoi(value);
        // New directional threshold settings
        else if (key == "NorthYFloor")
            NorthYFloor = std::stof(value);
        else if (key == "NorthYCeil")
            NorthYCeil = std::stof(value);
        else if (key == "NorthXFloor")
            NorthXFloor = std::stof(value);
        else if (key == "NorthXCeil")
            NorthXCeil = std::stof(value);
        else if (key == "WestYCeil")
            WestYCeil = std::stof(value);
        else if (key == "WestYFloor")
            WestYFloor = std::stof(value);
        else if (key == "WestXFloor")
            WestXFloor = std::stof(value);
        else if (key == "WestXCeil")
            WestXCeil = std::stof(value);
        else if (key == "EastYCeil")
            EastYCeil = std::stof(value);
        else if (key == "EastYFloor")
            EastYFloor = std::stof(value);
        else if (key == "EastXFloor")
            EastXFloor = std::stof(value);
        else if (key == "EastXCeil")
            EastXCeil = std::stof(value);
        else if (key == "SouthYCeil")
            SouthYCeil = std::stof(value);
        else if (key == "SouthYFloor")
            SouthYFloor = std::stof(value);
        else if (key == "SouthXFloor")
            SouthXFloor = std::stof(value);
        else if (key == "SouthXCeil")
            SouthXCeil = std::stof(value);
        // New color and shape settings
        else if (key == "secondCrossColorR")
            secondCrossColor.r = (Uint8)std::stoi(value);
        else if (key == "secondCrossColorG")
            secondCrossColor.g = (Uint8)std::stoi(value);
        else if (key == "secondCrossColorB")
            secondCrossColor.b = (Uint8)std::stoi(value);
        else if (key == "firstCrossColorR")
            firstCrossColor.r = (Uint8)std::stoi(value);
        else if (key == "firstCrossColorG")
            firstCrossColor.g = (Uint8)std::stoi(value);
        else if (key == "firstCrossColorB")
            firstCrossColor.b = (Uint8)std::stoi(value);
        else if (key == "firstCrossColorA")
            firstCrossColor.a = (Uint8)std::stoi(value);
        else if (key == "secondGlowColorR")
            secondGlowColor.r = (Uint8)std::stoi(value);
        else if (key == "secondGlowColorG")
            secondGlowColor.g = (Uint8)std::stoi(value);
        else if (key == "secondGlowColorB")
            secondGlowColor.b = (Uint8)std::stoi(value);
        else if (key == "shapeIndex")
            shapeIndex = std::stoi(value);
        // New corner settings
        else if (key == "CornerMax")
            CornerMax = std::stoi(value);
        else if (key == "CornerMin")
            CornerMin = std::stoi(value);
        else if (key == "ButtonSetIndex")
            buttonSetIndex = std::stoi(value);
        else if (key == "ToonIndex")
            toonSetIndex = std::stoi(value);
        else if (key == "fillR")
            ringFill.r = (Uint8)std::stoi(value);
        else if (key == "fillG")
            ringFill.g = (Uint8)std::stoi(value);
        else if (key == "fillB")
            ringFill.b = (Uint8)std::stoi(value);
        else if (key == "fillA")
            ringFill.a = (Uint8)std::stoi(value);
        else if (key == "staticButtonOverlaySwitch")
            staticButtonOverlaySwitch = (std::stoi(value) == 1);
        else if (key == "staticButtonOpacity")
            staticButtonOpacity = std::stoi(value);
        else if (key == "buttonSetLayoutIndex")
            buttonSetLayoutIndex = std::stoi(value);
    }

}
void saveSettings()
{
    std::ofstream file("settings.ini");
    if (file.is_open())
    {
        file << "showCoordinatesSwitch=" << (showCoordinatesSwitch ? "1" : "0") << "\n";
        file << "showMSpeedSwitch=" << (showMSpeedSwitch ? "1" : "0") << "\n";
        file << "toonSwitch=" << (toonSwitch ? "1" : "0") << "\n";
        // Removed: stickSensitivitySwitch
        file << "dPadSwitch=" << (dPadSwitch ? "1" : "0") << "\n";
        file << "circleSize=" << circleSize << "\n";
        file << "joystickCircleColorR=" << (int)joystickCircleColor.r << "\n";
        file << "joystickCircleColorG=" << (int)joystickCircleColor.g << "\n";
        file << "joystickCircleColorB=" << (int)joystickCircleColor.b << "\n";
        file << "pulseRate=" << pulseRate << "\n";
        file << "pulseAmplitude=" << pulseAmplitude << "\n";
        file << "glowMinAlpha=" << glowMinAlpha << "\n";
        file << "joystickGlowColorR=" << (int)joystickGlowColor.r << "\n";
        file << "joystickGlowColorG=" << (int)joystickGlowColor.g << "\n";
        file << "joystickGlowColorB=" << (int)joystickGlowColor.b << "\n";
        file << "thickLine=" << (thickLine ? "1" : "0") << "\n";
        file << "stickR=" << stickR << "\n";
        file << "stickG=" << stickG << "\n";
        file << "stickB=" << stickB << "\n";
        file << "bgR=" << bgR << "\n";
        file << "bgG=" << bgG << "\n";
        file << "bgB=" << bgB << "\n";
        // Outer ring settings
        file << "ringOuterRadius=" << ringOuterRadius << "\n";
        file << "ringThickness=" << ringThickness << "\n";
        file << "ringColorR=" << (int)ringColor.r << "\n";
        file << "ringColorG=" << (int)ringColor.g << "\n";
        file << "ringColorB=" << (int)ringColor.b << "\n";
        file << "ringOutlineColorR=" << (int)ringOutlineColor.r << "\n";
        file << "ringOutlineColorG=" << (int)ringOutlineColor.g << "\n";
        file << "ringOutlineColorB=" << (int)ringOutlineColor.b << "\n";
        // Second ring settings
        file << "secondRingOuterRadius=" << secondRingOuterRadius << "\n";
        file << "secondRingThickness=" << secondRingThickness << "\n";
        file << "secondRingColorR=" << (int)secondRingColor.r << "\n";
        file << "secondRingColorG=" << (int)secondRingColor.g << "\n";
        file << "secondRingColorB=" << (int)secondRingColor.b << "\n";
        file << "secondRingOutlineColorR=" << (int)secondRingOutlineColor.r << "\n";
        file << "secondRingOutlineColorG=" << (int)secondRingOutlineColor.g << "\n";
        file << "secondRingOutlineColorB=" << (int)secondRingOutlineColor.b << "\n";
        // New directional threshold settings
        file << "NorthYFloor=" << NorthYFloor << "\n";
        file << "NorthYCeil=" << NorthYCeil << "\n";
        file << "NorthXFloor=" << NorthXFloor << "\n";
        file << "NorthXCeil=" << NorthXCeil << "\n";
        file << "WestYCeil=" << WestYCeil << "\n";
        file << "WestYFloor=" << WestYFloor << "\n";
        file << "WestXFloor=" << WestXFloor << "\n";
        file << "WestXCeil=" << WestXCeil << "\n";
        file << "EastYCeil=" << EastYCeil << "\n";
        file << "EastYFloor=" << EastYFloor << "\n";
        file << "EastXFloor=" << EastXFloor << "\n";
        file << "EastXCeil=" << EastXCeil << "\n";
        file << "SouthYCeil=" << SouthYCeil << "\n";
        file << "SouthYFloor=" << SouthYFloor << "\n";
        file << "SouthXFloor=" << SouthXFloor << "\n";
        file << "SouthXCeil=" << SouthXCeil << "\n";
        // New color and shape settings
        file << "secondCrossColorR=" << (int)secondCrossColor.r << "\n";
        file << "secondCrossColorG=" << (int)secondCrossColor.g << "\n";
        file << "secondCrossColorB=" << (int)secondCrossColor.b << "\n";
        file << "firstCrossColorR=" << (int)firstCrossColor.r << "\n";
        file << "firstCrossColorG=" << (int)firstCrossColor.g << "\n";
        file << "firstCrossColorB=" << (int)firstCrossColor.b << "\n";
        file << "firstCrossColorA=" << (int)firstCrossColor.a << "\n";
        file << "secondGlowColorR=" << (int)secondGlowColor.r << "\n";
        file << "secondGlowColorG=" << (int)secondGlowColor.g << "\n";
        file << "secondGlowColorB=" << (int)secondGlowColor.b << "\n";
        file << "shapeIndex=" << shapeIndex << "\n";
        // New corner settings
        file << "CornerMax=" << CornerMax << "\n";
        file << "CornerMin=" << CornerMin << "\n";
        file << "ButtonSetIndex=" << buttonSetIndex << "\n";
        file << "ToonIndex=" << toonSetIndex << "\n";
        file << "fillR=" << (int)ringFill.r << "\n";
        file << "fillG=" << (int)ringFill.g << "\n";
        file << "fillB=" << (int)ringFill.b << "\n";
        file << "fillA=" << (int)ringFill.a << "\n";
        file << "staticButtonOverlaySwitch=" << (staticButtonOverlaySwitch ? "1" : "0") << "\n";
        file << "staticButtonOpacity=" << staticButtonOpacity << "\n";
        file << "buttonSetLayoutIndex=" << buttonSetLayoutIndex << "\n";
        file.close();
    }
}

void setIcon()
{
#include "icon.c"
    Uint32 rmask = 0x000000ff, gmask = 0x0000ff00, bmask = 0x00ff0000, amask;
    amask = (imgIcon.bytes_per_pixel == 3) ? 0 : 0xff000000;
    SDL_Surface* icon = SDL_CreateRGBSurfaceFrom((void*)imgIcon.pixel_data,
        imgIcon.width, imgIcon.height,
        imgIcon.bytes_per_pixel * 8,
        imgIcon.bytes_per_pixel * imgIcon.width,
        rmask, gmask, bmask, amask);
    SDL_SetWindowIcon(sdlWindow, icon);
    SDL_FreeSurface(icon);
}

SDL_GameController* controller = nullptr;

static void speedCheck(float cordX, float cordY)
{
    mSpeed = false;
    float crdX = cordX * 128;
    float crdY = cordY * 128;
    int crdXrounded = (int)crdX;
    int crdYrounded = (int)crdY;

    
    //50 Stick Sense
    if (crdY >= NorthYFloor && crdY <= NorthYCeil && crdX >= NorthXFloor && crdX <= NorthXCeil) { mSpeed = true; } // Handles Northern Slice
    if (crdY <= SouthYCeil && crdY >= SouthYFloor && crdX >= SouthXFloor && crdX <= SouthXCeil) { mSpeed = true; } // Handles Southern Slice
    if (crdX >= EastXFloor && crdX <= EastXCeil && crdY >= EastYFloor && crdY <= EastYCeil) { mSpeed = true; } // Handles Eastern Slice
    if (crdX <= WestXCeil && crdX >= WestXFloor && crdY >= WestYFloor && crdY <= WestYCeil) { mSpeed = true; } // Handles Western Slice
    if ((crdXrounded >= CornerMin && crdXrounded <= CornerMax) && (crdYrounded >= CornerMin && crdYrounded <= CornerMax)) { mSpeed = true; }  // Up-Right case 
    if ((crdXrounded <= CornerMinN && crdXrounded >= CornerMaxN) && (crdYrounded >= CornerMin && crdYrounded <= CornerMax)) { mSpeed = true; }  // Up-Left case 
    if ((crdXrounded >= CornerMin && crdXrounded <= CornerMax) && (crdYrounded <= CornerMinN && crdYrounded >= CornerMaxN)) { mSpeed = true; }  // Down-Right case 
    if ((crdXrounded <= CornerMinN && crdXrounded >= CornerMaxN) && (crdYrounded <= CornerMinN && crdYrounded >= CornerMaxN)) { mSpeed = true; }  // Down-Left case 
    
    //uncomment to  debug MSpeed issues
    // printf("SpeedCheck - crdX: %.2f, crdY: %.2f, Rounded: (%d, %d), Corners: [CornerMin: %d, CornerMax: %d, CornerMinN: %d, CornerMaxN: %d], mSpeed: %s\n", crdX, crdY, crdXrounded, crdYrounded, CornerMin, CornerMax, CornerMinN, CornerMaxN, (mSpeed ? "true" : "false"));

}
void pollAndUpdateGameController()
{
    char a = 0, b = 0, y = 0, x = 0, s = 0, r = 0, l = 0;
    float joyX = 0.0f, joyY = 0.0f;
    SDL_GameControllerUpdate();
    if (SDL_GameControllerGetAttached(controller) == SDL_FALSE)
    {
        controllerIsConnected = false;
        int numJoysticks = SDL_NumJoysticks();
        for (int i = 0; i < numJoysticks; i++)
        {
            if (SDL_IsGameController(i) != SDL_FALSE)
            {
                controller = SDL_GameControllerOpen(i);
                if (SDL_GameControllerGetAttached(controller) != SDL_FALSE)
                    break;
            }
        }
    }
    else
    {
        controllerIsConnected = true;
        a = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A);
        b = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B);
        y = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y);
        x = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_X);
        s = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_START);
        r = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
        l = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
        char du = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP);
        char dd = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        char dl = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        char dr = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        int leftX = (int)SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
        int leftY = (int)SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
        int triggerL = (int)SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
        int triggerR = (int)SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
        if (leftX > 0) leftX++;
        if (leftY > 0) leftY++;
        if (triggerL > 0) triggerL++;
        if (triggerR > 0) triggerR++;
        joyX = leftX / 32768.0f;
        joyY = -leftY / 32768.0f;
        float trigL = triggerL / 32768.0f;
        float trigR = triggerR / 32768.0f;
        if (trigL > 0.5f) tlFlag = true;
        if (trigR > 0.5f) trFlag = true;
        pRight = (dr != 0) ? 1 : 0;
        pLeft = (dl != 0) ? 1 : 0;
        pUp = (du != 0) ? 1 : 0;
        pDown = (dd != 0) ? 1 : 0;
        if (dPadSwitch)
        {
            if (dr != 0)
                joyX = 1.0f;
            else if (dl != 0)
                joyX = -1.0f;
            if (du != 0)
                joyY = 1.0f;
            else if (dd != 0)
                joyY = -1.0f;
        }
    }
    A = a; B = b; Y = y; X = x; S = s; R = r; L = l;
    LT = tlFlag; RT = trFlag;
    trFlag = false; tlFlag = false;
    int rawJoyX = (int)(joyX * 128);
    int rawJoyY = (int)(joyY * 128);
    float smoothingFactor = 0.7f;
    smoothedJoyX += (rawJoyX - smoothedJoyX) * smoothingFactor;
    smoothedJoyY += (rawJoyY - smoothedJoyY) * smoothingFactor;
    JoyX = (int)smoothedJoyX;
    JoyY = (int)smoothedJoyY;
    mSpeed = false;
    if (showMSpeedSwitch) { speedCheck(joyX, joyY);}
    if (showCoordinatesSwitch) { updateText((joyX * 128), (joyY * 128)); }
}

void UpdateAnimation()
{
    // Calculate the current animation frame index (0-7)
    animationIndexNumber = (SDL_GetTicks64() / 120) % 8;

    // Instead of reloading, simply update your displayed frame pointers:
    if (toonSwitch)
        imgBaseToon = toonFrames[animationIndexNumber];

    if (mSpeed)
        imgBaseToonM = toonMFrames[animationIndexNumber];

    // Update the text surfaces as before.
    surfaceMessage = TTF_RenderText_Solid(Sans, messageToDisplay.c_str(), White);
    surfaceMessage2 = TTF_RenderText_Solid(Sans, messageToDisplay2.c_str(), White);

    recMessage.w = surfaceMessage->w;
    recMessage2.w = surfaceMessage2->w;

    if (texMessage != nullptr) { SDL_DestroyTexture(texMessage); texMessage = nullptr; }
    if (texMessage2 != nullptr) { SDL_DestroyTexture(texMessage2); texMessage2 = nullptr; }

    texMessage = SDL_CreateTextureFromSurface(sdlRenderer, surfaceMessage);
    texMessage2 = SDL_CreateTextureFromSurface(sdlRenderer, surfaceMessage2);

    SDL_FreeSurface(surfaceMessage);
    SDL_FreeSurface(surfaceMessage2);
}

void saveIndex()
{
    std::ofstream fileInd("Index.ini");
    fileInd << std::to_string(folderIndex);
    fileInd.close();
}
void updateUIBlockLayout() {
    // Update the offset based on the layout index.
    // (The values below are suggestions; adjust them to your desired look.)
    if (buttonSetLayoutIndex == 0 || buttonSetLayoutIndex == 4) {
        // Default: ring stays at (288,144) as before.
        blockOffsetX = 0;
    }
    else if (buttonSetLayoutIndex == 1 || buttonSetLayoutIndex == 3) {
        // Move the entire UI block to the left.
        // For instance, if you want the ring center to shift to about 150 on X:
        blockOffsetX = 132 - 288;  // e.g. -138
    }
    else if (buttonSetLayoutIndex == 2) {
        // Center the ring and S button in the window.
        // With a window width of 420, the horizontal center is 210.
        blockOffsetX = 212 - 288;  // e.g. -78
    }
    // If you ever need to adjust Y, update blockOffsetY here (for now we keep it 0).

    // Also update any dependent SDL_Rects.
    // For example, recBase (the rectangle for your toon animation) is originally defined as:
    // {152, 8, 272, 272} with the ring center at 288.
    // We calculate the offset from the ring center (288,144) to recBase.
    int baseOffsetX = 152 - 288; // e.g. -136
    int baseOffsetY = 8 - 144;   // e.g. -136
    recBase.x = (288 + blockOffsetX) + baseOffsetX;
    recBase.y = (144 + blockOffsetY) + baseOffsetY;
    // Similarly, update your coordinate text rectangles.
    // For instance, if recMessage was originally {184, 280, 0, 20},
    // then update its x position as:
    recMessage.x = 184 + blockOffsetX;
    recMessage2.x = 268 + blockOffsetX;
    recMessage3.x = 380 + blockOffsetX;
    // (Adjust these numbers as needed.)
    int s_offsetX = 376 - 288;  // 88
    int s_offsetY = 10 - 144;   // -134
    recS.x = (288 + blockOffsetX) + s_offsetX;
    recS.y = (144 + blockOffsetY) + s_offsetY;
}
void renderStaticOverlay()
{
    // Set static overlay as the target.
    SDL_SetRenderTarget(sdlRenderer, staticOverlayTexture);
    // Clear with a transparent background.
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 0);
    SDL_RenderClear(sdlRenderer);

    int centerX = 288 + blockOffsetX;
    int centerY = 144 + blockOffsetY;
    int firstCrossLength = secondRingOuterRadius + 4;  // Based on your current code

    switch (shapeIndex) {
    case 0: case 1: {


        // Draw a filled circle.
        DrawFilledCircle(sdlRenderer, centerX, centerY, ringOuterRadius, ringFill);
        // ----- Circular Ring Case -----
        // Draw the annulus normally.
        DrawThirdCrosshairAnnulusMirrored(sdlRenderer, centerX, centerY,
            ringOuterRadius, ringThickness,
            firstCrossThickness, firstCrossColor,
            thirdCrossAngleOffset);
        // Draw the first crosshair (8 rays).
        SDL_SetRenderDrawColor(sdlRenderer, firstCrossColor.r, firstCrossColor.g, firstCrossColor.b, firstCrossColor.a);
        for (int i = 0; i < 8; i++) {
            float angle = i * (M_PI / 4.0f);
            int x2 = centerX + (int)(firstCrossLength * cos(angle));
            int y2 = centerY + (int)(firstCrossLength * sin(angle));
            DrawThickLineRect(sdlRenderer, centerX, centerY, x2, y2, firstCrossThickness, firstCrossColor);
        }
        // Draw the plus sign crosshair
        // First, draw the outline using a thicker line:
        SDL_Color outlineColor = { 0, 0, 0, 255 };
        SDL_SetRenderDrawColor(sdlRenderer, outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
        DrawThickLine(sdlRenderer, centerX - secondCrossLength, centerY,
            centerX + secondCrossLength, centerY, secondCrossThickness + 2);
        DrawThickLine(sdlRenderer, centerX, centerY - secondCrossLength,
            centerX, centerY + secondCrossLength, secondCrossThickness + 2);
        // Next, add endpoint fills for the horizontal line:
        int capThickness = secondCrossThickness + 2;  // the thickness used for the outline
        // Left endpoint:
        SDL_Rect leftCap = { centerX - secondCrossLength - 1, centerY - capThickness / 2, 2, capThickness };
        SDL_RenderFillRect(sdlRenderer, &leftCap);
        // Right endpoint:
        SDL_Rect rightCap = { centerX + secondCrossLength, centerY - capThickness / 2, 2, capThickness };
        SDL_RenderFillRect(sdlRenderer, &rightCap);
        // For the vertical line endpoints:
        SDL_Rect topCap = { centerX - capThickness / 2, centerY - secondCrossLength - 1, capThickness, 2 };
        SDL_RenderFillRect(sdlRenderer, &topCap);
        // Bottom endpoint: remove the -1 offset
        SDL_Rect bottomCap = { centerX - capThickness / 2, centerY + secondCrossLength, capThickness, 2 };
        SDL_RenderFillRect(sdlRenderer, &bottomCap);

        // Now, draw the actual plus sign on top:
        SDL_SetRenderDrawColor(sdlRenderer, secondCrossColor.r, secondCrossColor.g, secondCrossColor.b, secondCrossColor.a);
        DrawThickLine(sdlRenderer, centerX - secondCrossLength, centerY,
            centerX + secondCrossLength, centerY, secondCrossThickness);
        DrawThickLine(sdlRenderer, centerX, centerY - secondCrossLength,
            centerX, centerY + secondCrossLength, secondCrossThickness);
    
          break;
    }
    
            case 2:
            {
                // ----- Octagon Ring Case -----
                // Draw the outer ring as an octagon.
                // Subtract 4 from the outer radius and 4 from the thickness.
                DrawOctagon(sdlRenderer, centerX, centerY, ringOuterRadius - 4, ringThickness - 4, ringColor);
                // Draw the first crosshair (8 rays).
                SDL_SetRenderDrawColor(sdlRenderer, firstCrossColor.r, firstCrossColor.g, firstCrossColor.b, firstCrossColor.a);
                for (int i = 0; i < 8; i++) {
                    float angle = i * (M_PI / 4.0f);
                    int x2 = centerX + (int)(firstCrossLength * cos(angle));
                    int y2 = centerY + (int)(firstCrossLength * sin(angle));
                    DrawThickLineRect(sdlRenderer, centerX, centerY, x2, y2, firstCrossThickness, firstCrossColor);
                }

                // Draw the plus sign crosshair
                // First, draw the outline using a thicker line:
                SDL_Color outlineColor = { 0, 0, 0, 255 };
                SDL_SetRenderDrawColor(sdlRenderer, outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
                DrawThickLine(sdlRenderer, centerX - secondCrossLength, centerY,
                    centerX + secondCrossLength, centerY, secondCrossThickness + 2);
                DrawThickLine(sdlRenderer, centerX, centerY - secondCrossLength,
                    centerX, centerY + secondCrossLength, secondCrossThickness + 2);
                // Next, add endpoint fills for the horizontal line:
                int capThickness = secondCrossThickness + 2;  // the thickness used for the outline
                // Left endpoint:
                SDL_Rect leftCap = { centerX - secondCrossLength - 1, centerY - capThickness / 2, 2, capThickness };
                SDL_RenderFillRect(sdlRenderer, &leftCap);
                // Right endpoint:
                SDL_Rect rightCap = { centerX + secondCrossLength, centerY - capThickness / 2, 2, capThickness };
                SDL_RenderFillRect(sdlRenderer, &rightCap);
                // For the vertical line endpoints:
                SDL_Rect topCap = { centerX - capThickness / 2, centerY - secondCrossLength - 1, capThickness, 2 };
                SDL_RenderFillRect(sdlRenderer, &topCap);
                // Bottom endpoint: remove the -1 offset
                SDL_Rect bottomCap = { centerX - capThickness / 2, centerY + secondCrossLength, capThickness, 2 };
                SDL_RenderFillRect(sdlRenderer, &bottomCap);

                // Now, draw the actual plus sign on top:
                SDL_SetRenderDrawColor(sdlRenderer, secondCrossColor.r, secondCrossColor.g, secondCrossColor.b, secondCrossColor.a);
                DrawThickLine(sdlRenderer, centerX - secondCrossLength, centerY,
                    centerX + secondCrossLength, centerY, secondCrossThickness);
                DrawThickLine(sdlRenderer, centerX, centerY - secondCrossLength,
                    centerX, centerY + secondCrossLength, secondCrossThickness);
                // Draw the inner ring as an octagon.
                DrawOctagon(sdlRenderer, centerX, centerY, secondRingOuterRadius - 4, secondRingThickness - 1, secondRingColor);
            }
            break;
            case 3: {

                    // ---- Filled Circle with Simple Plus Sign Crosshair ----
                    // Draw a filled circle.
                DrawFilledCircle(sdlRenderer, centerX, centerY, ringOuterRadius, ringFill);
                // Optionally, draw an outline for the circle.
                DrawCircleOutline(sdlRenderer, centerX, centerY, ringOuterRadius, ringOutlineColor);

                // Draw the plus sign crosshair
                // First, draw the outline using a thicker line:
                SDL_Color outlineColor = { 0, 0, 0, 255 };
                SDL_SetRenderDrawColor(sdlRenderer, outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
                DrawThickLine(sdlRenderer, centerX - secondCrossLength, centerY,
                    centerX + secondCrossLength, centerY, secondCrossThickness + 2);
                DrawThickLine(sdlRenderer, centerX, centerY - secondCrossLength,
                    centerX, centerY + secondCrossLength, secondCrossThickness + 2);
                // Next, add endpoint fills for the horizontal line:
                int capThickness = secondCrossThickness + 2;  // the thickness used for the outline
                // Left endpoint:
                SDL_Rect leftCap = { centerX - secondCrossLength - 1, centerY - capThickness / 2, 2, capThickness };
                SDL_RenderFillRect(sdlRenderer, &leftCap);
                // Right endpoint:
                SDL_Rect rightCap = { centerX + secondCrossLength, centerY - capThickness / 2, 2, capThickness };
                SDL_RenderFillRect(sdlRenderer, &rightCap);
                // For the vertical line endpoints:
                SDL_Rect topCap = { centerX - capThickness / 2, centerY - secondCrossLength - 1, capThickness, 2 };
                SDL_RenderFillRect(sdlRenderer, &topCap);
                // Bottom endpoint: remove the -1 offset
                SDL_Rect bottomCap = { centerX - capThickness / 2, centerY + secondCrossLength, capThickness, 2 };
                SDL_RenderFillRect(sdlRenderer, &bottomCap);

                // Now, draw the actual plus sign on top:
                SDL_SetRenderDrawColor(sdlRenderer, secondCrossColor.r, secondCrossColor.g, secondCrossColor.b, secondCrossColor.a);
                DrawThickLine(sdlRenderer, centerX - secondCrossLength, centerY,
                    centerX + secondCrossLength, centerY, secondCrossThickness);
                DrawThickLine(sdlRenderer, centerX, centerY - secondCrossLength,
                    centerX, centerY + secondCrossLength, secondCrossThickness);
                }
                  break;
            case 4:
            {

                // Draw a filled circle.
                DrawFilledCircle(sdlRenderer, centerX, centerY, ringOuterRadius, ringFill);

                // Draw the plus sign crosshair
                // First, draw the outline using a thicker line:
                SDL_Color outlineColor = { 0, 0, 0, 255 };
                SDL_SetRenderDrawColor(sdlRenderer, outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
                DrawThickLine(sdlRenderer, centerX - secondCrossLength, centerY,
                    centerX + secondCrossLength, centerY, secondCrossThickness + 2);
                DrawThickLine(sdlRenderer, centerX, centerY - secondCrossLength,
                    centerX, centerY + secondCrossLength, secondCrossThickness + 2);
                // Next, add endpoint fills for the horizontal line:
                int capThickness = secondCrossThickness + 2;  // the thickness used for the outline
                // Left endpoint:
                SDL_Rect leftCap = { centerX - secondCrossLength - 1, centerY - capThickness / 2, 2, capThickness };
                SDL_RenderFillRect(sdlRenderer, &leftCap);
                // Right endpoint:
                SDL_Rect rightCap = { centerX + secondCrossLength, centerY - capThickness / 2, 2, capThickness };
                SDL_RenderFillRect(sdlRenderer, &rightCap);
                // For the vertical line endpoints:
                SDL_Rect topCap = { centerX - capThickness / 2, centerY - secondCrossLength - 1, capThickness, 2 };
                SDL_RenderFillRect(sdlRenderer, &topCap);
                // Bottom endpoint: remove the -1 offset
                SDL_Rect bottomCap = { centerX - capThickness / 2, centerY + secondCrossLength, capThickness, 2 };
                SDL_RenderFillRect(sdlRenderer, &bottomCap);

                // Now, draw the actual plus sign on top:
                SDL_SetRenderDrawColor(sdlRenderer, secondCrossColor.r, secondCrossColor.g, secondCrossColor.b, secondCrossColor.a);
                DrawThickLine(sdlRenderer, centerX - secondCrossLength, centerY,
                    centerX + secondCrossLength, centerY, secondCrossThickness);
                DrawThickLine(sdlRenderer, centerX, centerY - secondCrossLength,
                    centerX, centerY + secondCrossLength, secondCrossThickness);
                break;
            }
            case 5:
            {
                // --- Base Drawing: Same as mode 4 (circular ring without the red X crosshair) ---

                // Draw a filled circle.
                DrawFilledCircle(sdlRenderer, centerX, centerY, ringOuterRadius, ringFill);


                // Draw the plus sign crosshair
                // First, draw the outline using a thicker line:
                SDL_Color outlineColor = { 0, 0, 0, 255 };
                SDL_SetRenderDrawColor(sdlRenderer, outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
                DrawThickLine(sdlRenderer, centerX - secondCrossLength, centerY,
                    centerX + secondCrossLength, centerY, secondCrossThickness + 2);
                DrawThickLine(sdlRenderer, centerX, centerY - secondCrossLength,
                    centerX, centerY + secondCrossLength, secondCrossThickness + 2);
                // Next, add endpoint fills for the horizontal line:
                int capThickness = secondCrossThickness + 2;  // the thickness used for the outline
                // Left endpoint:
                SDL_Rect leftCap = { centerX - secondCrossLength - 1, centerY - capThickness / 2, 2, capThickness };
                SDL_RenderFillRect(sdlRenderer, &leftCap);
                // Right endpoint:
                SDL_Rect rightCap = { centerX + secondCrossLength, centerY - capThickness / 2, 2, capThickness };
                SDL_RenderFillRect(sdlRenderer, &rightCap);
                // For the vertical line endpoints:
                SDL_Rect topCap = { centerX - capThickness / 2, centerY - secondCrossLength - 1, capThickness, 2 };
                SDL_RenderFillRect(sdlRenderer, &topCap);
                // Bottom endpoint: remove the -1 offset
                SDL_Rect bottomCap = { centerX - capThickness / 2, centerY + secondCrossLength, capThickness, 2 };
                SDL_RenderFillRect(sdlRenderer, &bottomCap);

                // Now, draw the actual plus sign on top:
                SDL_SetRenderDrawColor(sdlRenderer, secondCrossColor.r, secondCrossColor.g, secondCrossColor.b, secondCrossColor.a);
                DrawThickLine(sdlRenderer, centerX - secondCrossLength, centerY,
                    centerX + secondCrossLength, centerY, secondCrossThickness);
                DrawThickLine(sdlRenderer, centerX, centerY - secondCrossLength,
                    centerX, centerY + secondCrossLength, secondCrossThickness);

                // --- New Clock Markers: 12 medium lines and 4 small lines between each medium line ---
                // Define marker parameters. Adjust these constants as needed.
                const int mediumLineLength = 20;        // Length of major (medium) markers
                const int mediumLineThickness = 4;        // Thickness of medium markers
                const int smallLineLength = 4;  // Small markers are half as long
                const int smallLineThickness = 4;  // Optionally use half the thickness

                // Choose a color for the markers (for example, use firstCrossColor or define a new one)
                SDL_Color markerColor = ringColor;

                // The starting radius for these markers is the inner edge of the outer ring:
                int baseRadius = ringOuterRadius - ringThickness;

                // Draw 12 major markers (one every 30°)
                for (int i = 0; i < 12; i++) {
                    float angle = i * (M_PI / 6.0f);  // 360°/12 = 30° in radians

                    // Compute the start point on the inner edge of the ring.
                    int sx = centerX + (int)(baseRadius * cos(angle));
                    int sy = centerY + (int)(baseRadius * sin(angle));

                    // End point is mediumLineLength inward.
                    int ex = centerX + (int)((baseRadius - mediumLineLength) * cos(angle));
                    int ey = centerY + (int)((baseRadius - mediumLineLength) * sin(angle));

                    // Compute a perpendicular offset for the thickness.
                    float offsetX = (mediumLineThickness / 2.0f) * (-sin(angle));
                    float offsetY = (mediumLineThickness / 2.0f) * (cos(angle));

                    SDL_Point quad[4];
                    quad[0] = { sx + (int)round(offsetX), sy + (int)round(offsetY) };
                    quad[1] = { sx - (int)round(offsetX), sy - (int)round(offsetY) };
                    quad[2] = { ex - (int)round(offsetX), ey - (int)round(offsetY) };
                    quad[3] = { ex + (int)round(offsetX), ey + (int)round(offsetY) };

                    // Draw the filled marker.
                    DrawFilledQuad(sdlRenderer, quad, markerColor);
                }
                // Draw 4 small markers between each major marker.
                // In each 30° interval, divide the gap into 5 equal segments.
                // The small markers will be drawn at offsets: 1/5, 2/5, 3/5, and 4/5 of the interval.
                for (int i = 0; i < 12; i++) {
                    float baseAngle = i * (M_PI / 6.0f);
                    for (int j = 1; j <= 4; j++) {
                        // Angle offset: each small marker is separated by (π/6)/5 = π/30
                        float angle = baseAngle + j * (M_PI / 30.0f);

                        int sx = centerX + (int)(baseRadius * cos(angle));
                        int sy = centerY + (int)(baseRadius * sin(angle));
                        int ex = centerX + (int)((baseRadius - smallLineLength) * cos(angle));
                        int ey = centerY + (int)((baseRadius - smallLineLength) * sin(angle));

                        // Use a smaller thickness.
                        float offsetX = (smallLineThickness / 2.0f) * (-sin(angle));
                        float offsetY = (smallLineThickness / 2.0f) * (cos(angle));

                        SDL_Point quad[4];
                        quad[0] = { sx + (int)round(offsetX), sy + (int)round(offsetY) };
                        quad[1] = { sx - (int)round(offsetX), sy - (int)round(offsetY) };
                        quad[2] = { ex - (int)round(offsetX), ey - (int)round(offsetY) };
                        quad[3] = { ex + (int)round(offsetX), ey + (int)round(offsetY) };
                        DrawFilledQuad(sdlRenderer, quad, markerColor);
                    }
              
                }
                break;
            }
    // Add additional cases if needed.
    }

    // Reset render target to default (screen).
    SDL_SetRenderTarget(sdlRenderer, nullptr);
    staticOverlayDirty = false;
}



// Create a global DialogMenu instance.
DialogMenu dialogMenu;

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    HWND handle = GetConsoleWindow();
    ShowWindow(handle, SW_HIDE);

    SDL_SetHintWithPriority(SDL_HINT_GAMECONTROLLER_USE_BUTTON_LABELS, "0", SDL_HINT_OVERRIDE);
    SDL_SetHintWithPriority(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1", SDL_HINT_OVERRIDE);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Init(SDL_INIT_GAMECONTROLLER);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    SDL_JoystickEventState(SDL_ENABLE);


    sdlWindow = SDL_CreateWindow("Controller Input", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 420, 300, SDL_WINDOW_SHOWN);

    for (int rendererIndex = 0; rendererIndex < 100; rendererIndex++)
    {
        sdlRenderer = SDL_CreateRenderer(sdlWindow, rendererIndex, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (sdlRenderer == nullptr)
        {
            sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            break;
        }
        SDL_RendererInfo info;
        SDL_GetRendererInfo(sdlRenderer, &info);
        std::string name = info.name;
        if (name == "direct3d11")
            break;
        else
        {
            SDL_DestroyRenderer(sdlRenderer);
            sdlRenderer = nullptr;
        }
    }

    // Create an off-screen texture for static (pre-rendered) shapes
    staticOverlayTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 420, 300);
    SDL_SetTextureBlendMode(staticOverlayTexture, SDL_BLENDMODE_BLEND);


    SDL_Texture* textureTransparent = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 420, 300);
    SDL_SetTextureBlendMode(textureTransparent, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawBlendMode(sdlRenderer, SDL_BLENDMODE_BLEND);
    // Create an off-screen texture (ensure it has an alpha channel)
    SDL_Texture* offscreen = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 420, 300);
    SDL_SetTextureBlendMode(offscreen, SDL_BLENDMODE_BLEND);
    SDL_Texture* glowTexture = createGlowTexture(circleSize, pulseAmplitude, joystickGlowColor);
    setIcon();

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(sdlWindow, &wmInfo);
    HWND hwnd = wmInfo.info.win.window;
    SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MINIMIZEBOX);
    SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);


    loadSettings();
    updateButtonSetLayout();
    updateUIBlockLayout();

    ringTexture = createRingTexture(288 + blockOffsetX, 144 + blockOffsetY, ringOuterRadius, ringThickness, ringColor, ringOutlineColor);
    ringLayerTexture = createRingTexture(288 + blockOffsetX, 144 + blockOffsetY, ringOuterRadius, ringThickness, ringColor, ringOutlineColor);
    ringInnerLayerTexture = createRingTexture(288 + blockOffsetX , 144 + blockOffsetY, secondRingOuterRadius, secondRingThickness, secondRingColor, secondRingOutlineColor);
    updateRingTextures();

    CornerMinN = (CornerMin * -1);
    CornerMaxN = (CornerMax * -1);
    folderIndex = 0;
    std::vector<std::string> indexLines = readFileLines("Index.ini");
    if (indexLines.size() == 1)
        folderIndex = std::stoi(indexLines[0]);

    loadImages();
    buttonSetFolders = getButtonSetFolders("./sets");
    if (buttonSetFolders.empty())
    {
        printf("No button sets found in ./sets/\n");
        // You might want to exit or fallback to a default.
    }
    else
    {
        printf("Found %d button sets.\n", (int)buttonSetFolders.size());
        loadButtonImages();
    }
    toonSetFolders = getToonSetFolders("./toons");
    if (toonSetFolders.empty())
    {
        printf("No toon sets found in ./toons/\n");
        // Optionally exit or fall back to a default set.
    }
    else
    {
        printf("Found %d toon sets.\n", (int)toonSetFolders.size());
        loadToonImages();
    }

    // Initialize the dialog menu with our font.
    dialogMenu.init(SansMenu);

    surfaceMessage3 = TTF_RenderText_Solid(Sans, messageToDisplay3.c_str(), White);
    texMessage3 = SDL_CreateTextureFromSurface(sdlRenderer, surfaceMessage3);

    bool running = true;
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            timerValue = SDL_GetTicks() / 100;
            // Pass events for the dialog window to the dialog menu.
            if (dialogMenu.isActive())
                dialogMenu.handleEvent(e);
            else {
                switch (e.type)
                {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                {
                    SDL_KeyboardEvent* keyEv = (SDL_KeyboardEvent*)&e;
                    if (keyEv->keysym.sym == SDLK_UP)
                    {
                        toonSetIndex--;
                        if (toonSetIndex < 0)
                            toonSetIndex = toonSetFolders.size() - 1;
                        loadToonImages();
                    }
                    else if (keyEv->keysym.sym == SDLK_DOWN)
                    {
                        toonSetIndex++;
                        if (toonSetIndex >= (int)toonSetFolders.size())
                            toonSetIndex = 0;
                        loadToonImages();
                    }
                    if (keyEv->keysym.sym == SDLK_LEFT)
                    {
                        buttonSetIndex--;
                        if (buttonSetIndex < 0)
                            buttonSetIndex = buttonSetFolders.size() - 1;
                        loadButtonImages();
                    }
                    else if (keyEv->keysym.sym == SDLK_RIGHT)
                    {
                        buttonSetIndex++;
                        if (buttonSetIndex >= buttonSetFolders.size())
                            buttonSetIndex = 0;
                        loadButtonImages();
                    }
                    if (keyEv->keysym.sym == SDLK_F1)
                        showCoordinatesSwitch = !showCoordinatesSwitch;
                    if (keyEv->keysym.sym == SDLK_F2)
                        toonSwitch = !toonSwitch;
                    if (keyEv->keysym.sym == SDLK_F3)
                        showMSpeedSwitch = !showMSpeedSwitch;
                    if (keyEv->keysym.sym == SDLK_F4)
                        dPadSwitch = !dPadSwitch;
                    if (keyEv->keysym.sym == SDLK_F5) {
                        if (shapeIndex < shapeIndexMaximum) { shapeIndex = shapeIndex + 1; }
                        else {
                            shapeIndex = 0;
                        }
                        updateRingTextures();
                        staticOverlayDirty = true;

                    }
                    if (keyEv->keysym.sym == SDLK_F6) {
                        staticButtonOverlaySwitch = !staticButtonOverlaySwitch;
                        // Mark the static overlay as dirty so it gets redrawn with the new button view.
                        staticOverlayDirty = true;
                    }
                    if (keyEv->keysym.sym == SDLK_F7) {
                        // Cycle through the 3 layouts: 0, 1, 2.
                        buttonSetLayoutIndex = (buttonSetLayoutIndex + 1) % 5;
                        updateButtonSetLayout();
                        updateUIBlockLayout();
                        // Mark the static overlay as dirty if your buttons are rendered
                        // into that off-screen texture.
                        staticOverlayDirty = true;
                    }
                    // h key opens the dialog menu.
                    if (keyEv->keysym.sym == SDLK_m)
                    {
                        dialogMenu.setActive(true);
                        dialogMenu.refreshFields();
                    }
                    break;
                }
                default:
                    break;
                }
            }
        }

        pollAndUpdateGameController();

        // Check if the static overlay should be updated:
        // Update every frame if the settings dialog is active, otherwise only when marked dirty.
        if (staticOverlayDirty || dialogMenu.isActive()) {
            renderStaticOverlay();
        }

        // Render objects to the off-screen texture
        SDL_SetRenderTarget(sdlRenderer, offscreen);
        SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 0);  // clear with transparent
        SDL_RenderClear(sdlRenderer);

        SDL_RenderCopy(sdlRenderer, staticOverlayTexture, nullptr, nullptr);


        if (showMSpeedSwitch || toonSwitch)
            UpdateAnimation();
        if (staticButtonOverlaySwitch) {
            // Set the alpha mod for each button texture.
            SDL_SetTextureAlphaMod(imgA, staticButtonOpacity);
            SDL_SetTextureAlphaMod(imgB, staticButtonOpacity);
            SDL_SetTextureAlphaMod(imgX, staticButtonOpacity);
            SDL_SetTextureAlphaMod(imgY, staticButtonOpacity);
            SDL_SetTextureAlphaMod(imgL, staticButtonOpacity);
            SDL_SetTextureAlphaMod(imgR, staticButtonOpacity);
            SDL_SetTextureAlphaMod(imgLT, staticButtonOpacity);
            SDL_SetTextureAlphaMod(imgRT, staticButtonOpacity);
            SDL_SetTextureAlphaMod(imgS, staticButtonOpacity);
            SDL_SetTextureAlphaMod(imgUp, staticButtonOpacity);
            SDL_SetTextureAlphaMod(imgDown, staticButtonOpacity);
            SDL_SetTextureAlphaMod(imgLeft, staticButtonOpacity);
            SDL_SetTextureAlphaMod(imgRight, staticButtonOpacity);

            // Render the button textures in their designated positions.
            SDL_RenderCopy(sdlRenderer, imgA, nullptr, &recA);
            SDL_RenderCopy(sdlRenderer, imgB, nullptr, &recB);
            SDL_RenderCopy(sdlRenderer, imgX, nullptr, &recX);
            SDL_RenderCopy(sdlRenderer, imgY, nullptr, &recY);
            SDL_RenderCopy(sdlRenderer, imgL, nullptr, &recL);
            SDL_RenderCopy(sdlRenderer, imgR, nullptr, &recR);
            SDL_RenderCopy(sdlRenderer, imgLT, nullptr, &recLT);
            SDL_RenderCopy(sdlRenderer, imgRT, nullptr, &recRT);
            SDL_RenderCopy(sdlRenderer, imgS, nullptr, &recS);
            SDL_RenderCopy(sdlRenderer, imgUp, nullptr, &recUp);
            SDL_RenderCopy(sdlRenderer, imgDown, nullptr, &recDown);
            SDL_RenderCopy(sdlRenderer, imgLeft, nullptr, &recLeft);
            SDL_RenderCopy(sdlRenderer, imgRight, nullptr, &recRight);

            // Reset to full opacity for dynamic drawing:
            SDL_SetTextureAlphaMod(imgA, 255);
            SDL_SetTextureAlphaMod(imgB, 255);
            SDL_SetTextureAlphaMod(imgX, 255);
            SDL_SetTextureAlphaMod(imgY, 255);
            SDL_SetTextureAlphaMod(imgL, 255);
            SDL_SetTextureAlphaMod(imgR, 255);
            SDL_SetTextureAlphaMod(imgLT, 255);
            SDL_SetTextureAlphaMod(imgRT, 255);
            SDL_SetTextureAlphaMod(imgS, 255);
            SDL_SetTextureAlphaMod(imgUp, 255);
            SDL_SetTextureAlphaMod(imgDown, 255);
            SDL_SetTextureAlphaMod(imgLeft, 255);
            SDL_SetTextureAlphaMod(imgRight, 255);
        }

        if (showCoordinatesSwitch)
        {
            SDL_RenderCopy(sdlRenderer, texMessage, nullptr, &recMessage);
            SDL_RenderCopy(sdlRenderer, texMessage2, nullptr, &recMessage2);
        }

        if (showMSpeedSwitch)
        {
            SDL_RenderCopy(sdlRenderer, texMessage3, nullptr, &recMessage3);
        }

        if (mSpeed && showMSpeedSwitch)
        {
            // --- 1. Render the static outer ring (using your original ringTexture) ---
            SDL_SetTextureColorMod(ringTexture, joystickGlowColor.r, joystickGlowColor.g, joystickGlowColor.b);
            SDL_SetTextureAlphaMod(ringTexture, 255);
            int texW, texH;
            SDL_QueryTexture(ringTexture, nullptr, nullptr, &texW, &texH);
            SDL_Rect destStatic = { (288 + blockOffsetX) - texW / 2, (144 + blockOffsetY) - texH / 2, texW, texH };
            SDL_RenderCopyEx(sdlRenderer, ringTexture, nullptr, &destStatic, 0.0, nullptr, SDL_FLIP_NONE);

            // --- 2. Calculate scaling factor for the pulsing rings ---
            float t = fmod(SDL_GetTicks() * pulseRate, 2.0f);
            float interp = (t < 1.0f) ? t : (2.0f - t);
            int extra = (int)(pulseAmplitude * interp);
            int newRadius = ringOuterRadius + extra;
            float scaleFactor = (float)newRadius / (float)ringOuterRadius;

            // --- 3. Render the white pulsing ring using the white texture ---
            SDL_SetTextureColorMod(ringLayerTextureWhite, 255, 255, 255);
            SDL_SetTextureAlphaMod(ringLayerTextureWhite, 255);
            SDL_QueryTexture(ringLayerTextureWhite, nullptr, nullptr, &texW, &texH);
            SDL_Rect destPulsingWhite = { (288 + blockOffsetX) - (int)(texW * scaleFactor) / 2, (144 + blockOffsetY) - (int)(texH * scaleFactor) / 2,
                                          (int)(texW * scaleFactor), (int)(texH * scaleFactor) };
            SDL_RenderCopyEx(sdlRenderer, ringLayerTextureWhite, nullptr, &destPulsingWhite, 0.0, nullptr, SDL_FLIP_NONE);

            // --- 4. Compute the blended color ---
            SDL_Color blendedColor;
            blendedColor.r = (Uint8)(joystickGlowColor.r * (1 - interp) + secondGlowColor.r * interp);
            blendedColor.g = (Uint8)(joystickGlowColor.g * (1 - interp) + secondGlowColor.g * interp);
            blendedColor.b = (Uint8)(joystickGlowColor.b * (1 - interp) + secondGlowColor.b * interp);
            blendedColor.a = 255;

            // --- 5. Render the pulsing (scaling) outer ring with the blended color on top ---
            SDL_SetTextureColorMod(ringLayerTextureWhite, blendedColor.r, blendedColor.g, blendedColor.b);
            SDL_SetTextureAlphaMod(ringLayerTextureWhite, blendedColor.a);
            SDL_QueryTexture(ringLayerTextureWhite, nullptr, nullptr, &texW, &texH);
            SDL_Rect destPulsing = { (288 + blockOffsetX) - (int)(texW * scaleFactor) / 2, (144 + blockOffsetY) - (int)(texH * scaleFactor) / 2,
                                     (int)(texW * scaleFactor), (int)(texH * scaleFactor) };
            SDL_RenderCopyEx(sdlRenderer, ringLayerTextureWhite, nullptr, &destPulsing, 0.0, nullptr, SDL_FLIP_NONE);

            // --- Render the inner ring ---

            // (A) Render the inner ring base with joystickGlowColor:
            SDL_SetTextureColorMod(ringInnerLayerTexture, joystickGlowColor.r, joystickGlowColor.g, joystickGlowColor.b);
            SDL_SetTextureAlphaMod(ringInnerLayerTexture, 255);
            int innerW, innerH;
            SDL_QueryTexture(ringInnerLayerTexture, nullptr, nullptr, &innerW, &innerH);
            SDL_Rect destInnerBase = { (288 + blockOffsetX) - innerW / 2, (144+ blockOffsetY) - innerH / 2, innerW, innerH };
            SDL_RenderCopyEx(sdlRenderer, ringInnerLayerTexture, nullptr, &destInnerBase, 0.0, nullptr, SDL_FLIP_NONE);

            // Compute pulsing parameters for the inner ring (same as outer ring logic):
            int extraInner = (int)(pulseAmplitude * interp);
            int newInnerRadius = secondRingOuterRadius + extraInner;
            float innerScale = (float)newInnerRadius / (float)secondRingOuterRadius;

            // (B) Render the white pulsing inner ring base:
            SDL_SetTextureColorMod(ringInnerLayerTextureWhite, 255, 255, 255);
            SDL_SetTextureAlphaMod(ringInnerLayerTextureWhite, 255);
            SDL_QueryTexture(ringInnerLayerTextureWhite, nullptr, nullptr, &innerW, &innerH);
            SDL_Rect destInnerWhite = { (288 + blockOffsetX) - (int)(innerW * innerScale) / 2, (144 + blockOffsetY) - (int)(innerH * innerScale) / 2,
                                        (int)(innerW * innerScale), (int)(innerH * innerScale) };
            SDL_RenderCopyEx(sdlRenderer, ringInnerLayerTextureWhite, nullptr, &destInnerWhite, 0.0, nullptr, SDL_FLIP_NONE);

            // (C) Render the pulsing overlay with the blended color on top:
            SDL_SetTextureColorMod(ringInnerLayerTextureWhite, blendedColor.r, blendedColor.g, blendedColor.b);
            SDL_SetTextureAlphaMod(ringInnerLayerTextureWhite, blendedColor.a);
            SDL_QueryTexture(ringInnerLayerTextureWhite, nullptr, nullptr, &innerW, &innerH);
            SDL_Rect destInnerPulsing = { (288 + blockOffsetX) - (int)(innerW * innerScale) / 2, (144 + blockOffsetY) - (int)(innerH * innerScale) / 2,
                                          (int)(innerW * innerScale), (int)(innerH * innerScale) };
            SDL_RenderCopyEx(sdlRenderer, ringInnerLayerTextureWhite, nullptr, &destInnerPulsing, 0.0, nullptr, SDL_FLIP_NONE);

        }
        else
        {
            // When not in mSpeed mode, render both textures at normal scale without modulation:
            SDL_SetTextureColorMod(ringLayerTexture, 255, 255, 255);
            SDL_SetTextureAlphaMod(ringLayerTexture, 255);
            int texW, texH;
            SDL_QueryTexture(ringLayerTexture, nullptr, nullptr, &texW, &texH);
            SDL_Rect dest;
            dest.w = texW;
            dest.h = texH;
            dest.x = (288 + blockOffsetX) - dest.w / 2;
            dest.y = (144 + blockOffsetY) - dest.h / 2;
            SDL_RenderCopyEx(sdlRenderer, ringLayerTexture, nullptr, &dest, 0.0, nullptr, SDL_FLIP_NONE);

            // And the inner ring texture:
            SDL_SetTextureColorMod(ringInnerLayerTexture, 255, 255, 255);
            SDL_SetTextureAlphaMod(ringInnerLayerTexture, 255);
            SDL_QueryTexture(ringInnerLayerTexture, nullptr, nullptr, &texW, &texH);
            dest.w = texW;
            dest.h = texH;
            dest.x = (288 + blockOffsetX) - dest.w / 2;
            dest.y = (144 + blockOffsetY) - dest.h / 2;
            SDL_RenderCopyEx(sdlRenderer, ringInnerLayerTexture, nullptr, &dest, 0.0, nullptr, SDL_FLIP_NONE);

            // Draw the crosshair annulus over the rings
            SDL_SetRenderDrawColor(sdlRenderer, firstCrossColor.r, firstCrossColor.g, firstCrossColor.b, firstCrossColor.a);
            switch (shapeIndex) {
            case 0: case 2: {
               //DrawThirdCrosshairAnnulusMirrored(sdlRenderer, 288, 144, ringOuterRadius - 4, ringThickness, firstCrossThickness, firstCrossColor, thirdCrossAngleOffset);
                break;
            }
            case 1: {
                DrawThirdCrosshairAnnulusMirrored(sdlRenderer, (288 + blockOffsetX), (144 + blockOffsetY), ringOuterRadius, ringThickness, firstCrossThickness, firstCrossColor, thirdCrossAngleOffset);
                break;
            }
            default:
                break;
            }

        }
        




        if (toonSwitch)
        {
            if (!showMSpeedSwitch || !mSpeed)
                SDL_RenderCopy(sdlRenderer, imgBaseToon, nullptr, &recBase);
            else
                SDL_RenderCopy(sdlRenderer, imgBaseToonM, nullptr, &recBase);

        }

        int drawX = ((288 + blockOffsetX) + blockOffsetX) + JoyX;
        int drawY = ((144 + blockOffsetY) + blockOffsetY) - JoyY;
        float rad = std::fmin(std::sqrt((float)(JoyX * JoyX + JoyY * JoyY)), 128.0f);
        float ang = std::atan2((float)JoyY, (float)JoyX);
        int capX = (int)(rad * std::cos(ang));
        int capY = (int)(rad * std::sin(ang));
        int drawCapX = (288 + blockOffsetX) + capX;
        int drawCapY = (144 + blockOffsetY) - capY;

        SDL_Color stickLineColor;
        stickLineColor.r = stickR;
        stickLineColor.g = stickG;
        stickLineColor.b = stickB;
        stickLineColor.a = 255;

        //SDL_SetRenderDrawColor(sdlRenderer, stickR, stickG, stickB, 255);
        //SDL_RenderDrawLine(sdlRenderer, 288, 144, drawCapX, drawCapY);
        DrawThickLineRect(sdlRenderer, (288 + blockOffsetX), (144 + blockOffsetY), drawCapX, drawCapY,3,stickLineColor);

        if (mSpeed && showMSpeedSwitch)
        {
            // Draw the small circle on top (if needed)
            SDL_Color activeCircleColor = joystickGlowColor;
            activeCircleColor.a = 255;
            DrawFilledCircle(sdlRenderer, drawCapX, drawCapY, circleSize, activeCircleColor);
            SDL_Color outlineColor = { 0, 0, 0, 255 };
            DrawCircleOutline(sdlRenderer, drawCapX, drawCapY, circleSize, outlineColor);
        }
        else
        {
            DrawFilledCircle(sdlRenderer, drawCapX, drawCapY, circleSize, joystickCircleColor);
            SDL_Color outlineColor = { 0, 0, 0, 255 };
            DrawCircleOutline(sdlRenderer, drawCapX, drawCapY, circleSize, outlineColor);
        }

        if (A) SDL_RenderCopy(sdlRenderer, imgA, nullptr, &recA);
        if (B) SDL_RenderCopy(sdlRenderer, imgB, nullptr, &recB);
        if (X) SDL_RenderCopy(sdlRenderer, imgX, nullptr, &recX);
        if (Y) SDL_RenderCopy(sdlRenderer, imgY, nullptr, &recY);
        if (L) SDL_RenderCopy(sdlRenderer, imgL, nullptr, &recL);
        if (R) SDL_RenderCopy(sdlRenderer, imgR, nullptr, &recR);
        if (LT) SDL_RenderCopy(sdlRenderer, imgLT, nullptr, &recLT);
        if (RT) SDL_RenderCopy(sdlRenderer, imgRT, nullptr, &recRT);
        if (pUp) SDL_RenderCopy(sdlRenderer, imgUp, nullptr, &recUp);
        if (pDown) SDL_RenderCopy(sdlRenderer, imgDown, nullptr, &recDown);
        if (pLeft) SDL_RenderCopy(sdlRenderer, imgLeft, nullptr, &recLeft);
        if (pRight) SDL_RenderCopy(sdlRenderer, imgRight, nullptr, &recRight);
        if (S) SDL_RenderCopy(sdlRenderer, imgS, nullptr, &recS);

        if (SDL_GetTicks() / 100 <= 30)
            SDL_RenderCopy(sdlRenderer, imgHelp1, nullptr, &recFull);

        // Reset target to default
        SDL_SetRenderTarget(sdlRenderer, nullptr);

        // Now clear the main renderer with your background color
        SDL_SetRenderDrawColor(sdlRenderer, bgR, bgG, bgB, 0);
        SDL_RenderClear(sdlRenderer);

        // Render the off-screen texture on top of the background
        SDL_RenderCopy(sdlRenderer, offscreen, nullptr, nullptr);
        SDL_RenderPresent(sdlRenderer);



        // Render the separate settings window if open.
        if (dialogMenu.isActive())
            dialogMenu.render();

        SDL_Delay(16);
        if (folderIndexLast != folderIndex)
            reloadImages();
    }

    saveIndex();
    saveSettings();
    cleanUp();
    SDL_DestroyTexture(offscreen);
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
