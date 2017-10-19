//----------------------------------------------------------------------------------------------------------------------
// Memory dump window implementation
//----------------------------------------------------------------------------------------------------------------------

#include "debugger.h"
#include "spectrum.h"

#include <sstream>
#include <iomanip>

//----------------------------------------------------------------------------------------------------------------------
// Memory dump class
//----------------------------------------------------------------------------------------------------------------------

MemoryDumpWindow::MemoryDumpWindow(Spectrum& speccy)
    : SelectableWindow(speccy, 1, 1, 43, 20, "Memory Viewer", Colour::Black, Colour::White)
    , m_address(0)
    , m_gotoEditor(6, 2, 43, 1, Draw::attr(Colour::White, Colour::Magenta, false), false, 4, 0)
    , m_enableGoto(false)
{

}

void MemoryDumpWindow::onDraw(Draw& draw)
{
    u16 a = m_address;
    for (int i = 1; i < m_height - 1; ++i, a += 8)
    {
        using namespace std;
        stringstream ss;
        ss << setfill('0') << setw(4) << hex << uppercase << a << " : ";
        for (int b = 0; b < 8; ++b)
        {
            ss << setfill('0') << setw(2) << hex << uppercase << (int)m_speccy.peek(a + b) << " ";
        }
        ss << "  ";
        for (int b = 0; b < 8; ++b)
        {
            char ch = m_speccy.peek(a + b);
            ss << ((ch < 32 || ch > 127) ? '.' : ch);
        }
        draw.printString(m_x + 1, m_y + i, ss.str(), m_bkgColour);
    }

    if (m_enableGoto)
    {
        draw.attrRect(m_x + 1, m_y + 1, 4, 1, draw.attr(Colour::White, Colour::Magenta, true));
        draw.printString(m_x + 1, m_y + 1, "    ", draw.attr(Colour::White, Colour::Magenta, true));
        draw.printSquashedString(m_x + 1, m_y + 1, "Goto:", draw.attr(Colour::Yellow, Colour::Magenta, true));
        m_gotoEditor.render(draw, 0);
    }
}

void MemoryDumpWindow::onKey(sf::Keyboard::Key key, bool shift, bool ctrl, bool alt)
{
    if (!m_enableGoto || !m_gotoEditor.key(key, true, shift, ctrl, alt))
    {
        // An editor didn't handle the key so handle it here
        using K = sf::Keyboard::Key;
        switch (key)
        {
        case K::Up:
            m_address -= 8;
            break;

        case K::Down:
            m_address += 8;
            break;

        case K::PageUp:
            m_address -= (m_height - 2) * 8;
            break;

        case K::PageDown:
            m_address += (m_height - 2) * 8;
            break;

        case K::Escape:
            m_enableGoto = false;
            break;

        case K::G:
            m_gotoEditor.clear();
            m_enableGoto = true;
            break;

        default:
            break;
        }
    }
}

