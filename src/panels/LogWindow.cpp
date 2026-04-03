
#include "polyhydra/panels/LogWindow.h"

#include "polyhydra/input/Input.h"
#include "polyhydra/rendering/Renderer.h"
#include "polyhydra/util/ImGuiUtil.h"
#include <utility>

namespace polyhydra::Internal
{
LogWindow::LogWindow() = default;
float LogWindow::min_height = 24.0f;

void LogWindow::show(float max_x, float max_y, const std::shared_ptr<Renderer>& renderer)
{
    // set height and width to a third of the viewport
    m_width = ImGui::GetContentRegionAvailWidth() * m_size_factor;
    m_height = ImGui::GetWindowHeight() / 3.0f - (float)((int)ImGui::GetWindowHeight() % (int)min_height);

    // position above the arrow_button
    auto end = ImVec2(ImGui::GetStyle().FramePadding.x,
                      max_y - ImGui::GetFrameHeightWithSpacing() - ImGui::GetStyle().WindowPadding.y);
    auto start = ImVec2(end.x, end.y - m_height);

    if (m_visible)
    {
        // handle window dragging on its own
        auto screen_pos = ImGui::GetCursorScreenPos();
        auto mouse_pos = ImGui::GetMousePos();
        auto right_min = glm::vec2(start.x + m_width, end.y - m_height) + glm::vec2(screen_pos.x, 0.0f);
        auto right_max = glm::vec2(start.x + m_width, end.y + ImGui::GetFrameHeight()) + glm::vec2(screen_pos.x, 0.0f);
        float bias = 5.0f;

        if (mouse_pos.x >= right_min.x && mouse_pos.x <= right_min.x + bias)
        {
            if (mouse_pos.y >= right_min.y && mouse_pos.y <= right_max.y)
            {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
                ImGui::GetWindowDrawList()->AddLine({right_min.x, right_min.y + ImGui::GetStyle().WindowRounding},
                                                    {right_max.x, right_max.y - ImGui::GetStyle().WindowRounding},
                                                    ImColor(0.1f, 0.1f, 0.1f, 0.3f),
                                                    2.0f);
                if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    resizing = true;
                    renderer->input_blocking = true;
                }
            }
        }

        if (resizing)
        {
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                resizing = false;
                renderer->input_blocking = false;
            }
            else
            {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
                ImGui::GetWindowDrawList()->AddLine({right_min.x, right_min.y + ImGui::GetStyle().WindowRounding},
                                                    {right_max.x, right_max.y - ImGui::GetStyle().WindowRounding},
                                                    ImColor(0.1f, 0.1f, 0.1f, 0.3f),
                                                    2.0f);
                auto new_factor
                    = (mouse_pos.x - ImGui::GetStyle().FramePadding.x - screen_pos.x) / ImGui::GetContentRegionMax().x;
                if (std::abs(m_size_factor - new_factor) >= 0.001f)
                {
                    m_size_factor = new_factor;
                    ImGui::SetNextWindowFocus();
                    if (m_size_factor >= 0.9)
                        m_size_factor = 0.9;
                    if (m_size_factor <= 0.1)
                        m_size_factor = 0.1;
                }
            }
        }

        ImGui::SetCursorPos(ImVec2(end.x, end.y - m_height));

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.1f));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
        ImGui::BeginChild("child",
                          {m_width, m_height + ImGui::GetFrameHeight()},
                          false); //, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.0f));
        ImGui::BeginChildFrame(1,
                               {m_width, m_height},
                               ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground
                                   | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollbar);
        ImGui::BeginGroup();
        ImGui::PushStyleColor(ImGuiCol_TableBorderLight, ImVec4(0.1f, 0.1f, 0.1f, 0.1f));
        ImGui::BeginTable("Messages", 4, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollX);

        // show messages
        for (const auto& message : messages)
        {
            show_message(message);
        }

        if (m_adjust_scrollbar)
        {
            ImGui::SetScrollHereY(1.0f);
            m_adjust_scrollbar = false;
        }

        ImGui::EndTable();
        ImGui::EndGroup();

        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::EndChildFrame();

        ImGui::SetCursorPos(ImVec2(0.0f, m_height));
        if (ImGui::ArrowButton("##arrow", m_arrow_dir))
        {
            switch_arrow();
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        ImGui::EndChild();
    }
    else
    {
        ImGui::SetCursorPos(end);
        if (ImGui::ArrowButton("##arrow", m_arrow_dir))
        {
            switch_arrow();
        }
    }
}

void LogWindow::add_message(std::string msg, Type type)
{
    Message message = {};
    message.msg = std::move(msg);
    message.type = type;
    message.time = time(nullptr);
    messages.push_back(message);

    m_adjust_scrollbar = true;
}

void LogWindow::show_message(const Message& message)
{
    // Type | Time | Message
    // -----+------+---------
    // Type | Time | Message

    ImGui::PushID(message.msg.c_str());
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Dummy(ImVec2(0.0f, min_height));
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + min_height / 2.0f));
    ImGuiStyle* style = &ImGui::GetStyle();
    auto text_color = style->Colors[ImGuiCol_Text];
    switch (message.type)
    {
    case Info:
        style->Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImGuiUtil::icon("info2.png", ImGui::GetFontSize(), true);
        break;
    case Warning:
        style->Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
        ImGuiUtil::icon("alert.png", ImGui::GetFontSize(), true);
        break;
    case Error:
        style->Colors[ImGuiCol_Text] = ImVec4(1.0f, 0.0f, 0.0f, 1.00f);
        ImGuiUtil::icon("error.png", ImGui::GetFontSize(), true);
        break;
    }

    //        ImGui::TableSetColumnIndex(1);
    //        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + min_height / 2.0f));
    //        ImGui::SetNextItemWidth(ImGui::CalcTextSize(types[(int)message.type]).x);
    //        ImGui::Text("%s", types[(int)message.type]);

    ImGui::TableSetColumnIndex(1);
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + min_height / 2.0f));
    char buffer[9];
    strftime(buffer, 9, "%H:%M:%S", localtime(&message.time));
    ImGui::SetNextItemWidth(ImGui::CalcTextSize(buffer).x);
    ImGui::Text(" [%s] ", buffer);

    ImGui::TableSetColumnIndex(2);
    ImGui::SetNextItemWidth(ImGui::GetContentRegionMax().x);
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + min_height / 2.0f));
    ImGui::Text("%s", message.msg.c_str());
    ImGui::PopID();
    style->Colors[ImGuiCol_Text] = text_color;
}

void LogWindow::switch_arrow()
{

    m_adjust_scrollbar = true;
    if (m_arrow_dir == ImGuiDir_Up)
    {
        m_arrow_dir = ImGuiDir_Right;
        m_visible = false;
    }
    else
    {
        m_arrow_dir = ImGuiDir_Up;
        m_visible = true;
    }
}

void LogWindow::hide_log_window(bool hide)
{
    if ((hide && m_visible) || (!hide && !m_visible))
    {
        switch_arrow();
    }
}

void LogWindow::clear_logs()
{
    messages.clear();
}

} // namespace polyhydra::Internal
