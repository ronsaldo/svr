#ifndef _SVR_STATUS_BAR_HPP_
#define _SVR_STATUS_BAR_HPP_

#include <vector>
#include "SVR/Widget.hpp"

namespace SVR
{
DECLARE_CLASS(StatusBar)
DECLARE_CLASS(StatusBarEntry)

/**
 * Status bar entry
 */
class StatusBarEntry
{
public:
    const std::string &getText() const
    {
        return text;
    }

    void setText(const std::string &newText)
    {
        text = newText;
    }

    int getProportion() const
    {
        return proportion;
    }

    void setProportion(int newProportion)
    {
        proportion = newProportion;
    }

private:
    std::string text;
    int proportion;
};

/**
 * Status bar
 */
class StatusBar: public Widget
{
public:

    size_t getNumberOfEntries() const
    {
        return entries.size();
    }

    const StatusBarEntryPtr &getEntry(size_t index) const
    {
        return entries[index];
    }

    void addEntry(const StatusBarEntryPtr &entry)
    {
        entries.push_back(entry);
    }

    StatusBarEntryPtr addEntry(const std::string &text, int proportion)
    {
        auto entry = std::make_shared<StatusBarEntry> ();
        entry->setText(text);
        entry->setProportion(proportion);
        addEntry(entry);
        return entry;
    }

protected:

    int getTotalProportion() const
    {
        int res = 0;
        for(auto &entry: entries)
            res += entry->getProportion();
        return res;
    }

    virtual void drawContent(const RendererPtr &renderer)
    {
        clearBackground(renderer);

        auto position = glm::vec2(20.0, getHeight() / 3.0);
        auto increment = glm::vec2( (getWidth() - 40.0) / getTotalProportion(), 0.0);
        for(auto & entry: entries)
        {
            drawEntry(renderer, position, entry);
            position += increment*float(entry->getProportion());
        }
    }

    void drawEntry(const RendererPtr &renderer, const glm::vec2 &position, const StatusBarEntryPtr &entry)
    {
        renderer->drawText(position, entry->getText());
    }

    std::vector<StatusBarEntryPtr> entries;
};

} // namespace SVR

#endif //_SVR_STATUS_BAR_HPP_
