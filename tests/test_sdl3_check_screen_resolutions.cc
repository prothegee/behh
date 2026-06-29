#include <behh/pch.hh>

#include <SDL3/SDL.h>

#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace behh;

//////////////////////////////////////////////////////////////////

#if BEHH_USING_SDL3
struct TDisplayResolutionData {
    int32_t width;
    int32_t height;
    float64_t refresh_rate;
};

class CTestNativeResolution {
private:
    // num count of display to intialize
    int m_numDisplay;

    // represent each display resolution data for each first index
    // each child of index hold their own data
    std::vector<std::vector<TDisplayResolutionData>> m_displayResolutionsData;

public:
    CTestNativeResolution() = default;
    ~CTestNativeResolution() = default;

    ///////////////////////////////////////////////////////////////

    void initialize() {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::printf("fail to init sdl video\n");
        }

        SDL_DisplayID* pDisplay = SDL_GetDisplays(&m_numDisplay);

        if (m_numDisplay <= 0) {
            std::printf("ERROR: can't found displays count\n");
        } else {
            if (!pDisplay) {
                std::printf("ERROR: can't found displays id pointer count\n");
                return;
            } else {
                for (int32_t i = 0; i < m_numDisplay; i++) {
                    int displayModeNum;
                    SDL_DisplayID displayId = pDisplay[i];
                    std::vector<TDisplayResolutionData> displayResolutionsData;

                    auto pScreenDisplayMode = SDL_GetFullscreenDisplayModes(displayId, &displayModeNum);

                    if (!pScreenDisplayMode || displayModeNum <= 0) {
                        continue;
                    }

                    // append for each screen
                    for (int32_t j = 0; j < displayModeNum; j++) {
                        SDL_DisplayMode* pMode = pScreenDisplayMode[j];
                        TDisplayResolutionData displayResolutionData;

                        displayResolutionData.width = pMode->w;
                        displayResolutionData.height = pMode->h;
                        displayResolutionData.refresh_rate = pMode->refresh_rate;

                        displayResolutionsData.emplace_back(displayResolutionData);
                    }

                    // store to main available resolutions data
                    m_displayResolutionsData.emplace_back(displayResolutionsData);

                    // if (auto pDisplayMode = SDL_GetDesktopDisplayMode(displayId))
                    // {
                    //     std::string resolution = std::to_string(pDisplayMode->w) + "x" + std::to_string(pDisplayMode->h);
                    //     m_availableResolutions.emplace_back(resolution);
                    // } // only 1 where current is set
                }
            }
        }

        std::printf("CTestNativeResolution initialized\n");
    }

    void terminateAndShutdown() {
        SDL_Quit();

        std::printf("CTestNativeResolution terminated & shutdown\n");
    }

    ///////////////////////////////////////////////////////////////

    /**
     * @brief get each display resolution data
     *
     * @note each index represent display id
     *
     * @return std::vector<std::vector<TDisplayResolutionData>>
     */
    std::vector<std::vector<TDisplayResolutionData>> getDisplayResolutionsData() {
        return m_displayResolutionsData;
    }
};
#endif // BEHH_USING_SDL3

int main() {
#if BEHH_USING_SDL3
    CTestNativeResolution* pNativeRes = new CTestNativeResolution();

    pNativeRes->initialize();

    auto availableResolutions = pNativeRes->getDisplayResolutionsData();
    for (int32_t i = 0; i < availableResolutions.size(); i++) {
        std::printf("display #%d\n", i + 1);
        for (int32_t j = 0; j < availableResolutions[i].size(); j++) {
            std::ostringstream refreshRateStream;
            refreshRateStream << std::fixed << std::setprecision(3) << availableResolutions[i][j].refresh_rate;

            std::string resolutionPixel = std::to_string(availableResolutions[i][j].width) + "x" + std::to_string(availableResolutions[i][j].height) + " px";

            std::printf(
                "    - resolution #%d:\n        - resolution  : %s\n        - refresh rate: %s Hz\n",
                (j + 1),
                resolutionPixel.c_str(),
                refreshRateStream.str().c_str());
        }
    }

    pNativeRes->terminateAndShutdown();

    delete pNativeRes;
#endif // BEHH_USING_SDL3

    return 0;
}