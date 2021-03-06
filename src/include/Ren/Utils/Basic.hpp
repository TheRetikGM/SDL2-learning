#pragma once
#include <array>

#include "Ren/RenSDL/Context.hpp"

namespace Ren::Utils
{
    template<typename T, std::size_t MAX_ENTRIES>
    class CyclingArray
    {
        T mEntries[MAX_ENTRIES];
        uint32_t mStart = 0;
        uint32_t mEnd = 0;
    public:
        inline size_t size() { return mEnd - mStart; }
        void push_back(const T& entry) 
        {
            if (mEnd >= MAX_ENTRIES)
                mStart++;

            mEntries[mEnd % MAX_ENTRIES] = entry;
            mEnd++;

            if (mStart >= MAX_ENTRIES)
            {
                mStart = 0;
                mEnd = MAX_ENTRIES;
            }
        }
        inline void clear() { mStart = mEnd = 0; }
        inline T& operator[] (const int& index) { return mEntries[(mStart + index) % MAX_ENTRIES]; }
        inline T& at(const int& index) { return (*this)[index]; }

        class iterator
        {
            uint32_t index = 0;
            CyclingArray* pArr = nullptr;
        public:
            iterator() = default;
            iterator(uint32_t index, CyclingArray* array) : index(index), pArr(array) {}

            bool operator==(const iterator& it) { return index == it.index; }
            bool operator!=(const iterator& it) { return index != it.index; }
            iterator& operator++() { index++; return *this; }
            iterator& operator--() { index--; return *this; }
            T& operator->() { return (*pArr)[index]; }
            T& operator*() { return (*pArr)[index]; }
        };

        iterator begin() { return {0, this}; }
        iterator end() { return {(uint32_t)size(), this}; }
    };

    class FpsCounter
    {
        using ValueArr = CyclingArray<float, 288>;
    public:

        void Update(float dt)
        {
            m_currentSampleTime += dt;
            m_nSamples++;
            m_values.push_back(1.0f / dt);
            
            if (m_currentSampleTime >= m_sampleTime)
            {;
                m_fps = m_nSamples / m_currentSampleTime;
                reset();
            }
        }
        inline float GetFps() { return m_fps; }
        inline void SetSampleTime(float n_sec) { m_sampleTime = n_sec; reset(); }

        void DrawPlot(Ren::ImGuiContext& context, glm::ivec2 win_size)
        {
            struct Funcs
            {
                static float value_getter(void* p_arr, int i)
                {
                    ValueArr* arr = (ValueArr*)p_arr;
                    return arr->at(i);
                }
            };

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
            ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
            static bool lock_pos = true;
            if (lock_pos)
                ImGui::SetNextWindowPos(ImVec2(10.0f, win_size.y - 105.0f));
            if (ImGui::Begin("Ren::Utils::FpsCounter", nullptr, window_flags))
            {
                //ImGui::Text("FPS: %.1f", m_fps);
                char avg_fps[20];
                std::sprintf(avg_fps, "avg. %.1f", m_fps);
                ImGui::PlotLines("FPS", &Funcs::value_getter, &m_values, m_values.size(), 0, avg_fps, 0.0f, 200.0f, ImVec2(0.0f, 80.0f));
                
                if (ImGui::BeginPopupContextWindow())
                {
                    ImGui::MenuItem("Lock position", NULL, &lock_pos);
                    ImGui::EndPopup();
                }
            }
            ImGui::End();
        }

    private:
        ValueArr m_values;
        float m_sampleTime = 2.0f;
        float m_currentSampleTime = 0.0f;
        int m_nSamples = 0;

        float m_fps{ 0.0f };

        void reset()
        {
            m_currentSampleTime = 0.0f;
            m_nSamples = 0;
        }

    };

    
} // namespace Ren::Utils
