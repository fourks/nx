//----------------------------------------------------------------------------------------------------------------------
// Disassembly Window
//----------------------------------------------------------------------------------------------------------------------

#include "debugger.h"
#include "spectrum.h"

#include <cassert>

//----------------------------------------------------------------------------------------------------------------------
// Disassembler class
//----------------------------------------------------------------------------------------------------------------------

#include "disasm.h"

DisassemblyWindow::DisassemblyWindow(Spectrum& speccy)
    : SelectableWindow(speccy, 1, 22, 43, 30, "Disassembly", Colour::Black, Colour::White)
    , m_address(0)
    , m_topAddress(0)
{
    adjustBar();
}

void DisassemblyWindow::adjustBar()
{
    if (m_address < m_topAddress)
    {
        setView(m_address);
    }

    // Calculate which row our bar is on.
    int row = 0;
    u16 a = m_topAddress;
    Disassembler d;

    int index = findViewAddress(a);

    for (row = 1; row < m_height - 1; ++row)
    {
        if (index == -1)
        {
            m_viewedAddresses.clear();
            index = 0;
        }
        if (index == m_viewedAddresses.size())
        {
            m_viewedAddresses.push_back(a);
        }
        ++index;

        if (a >= m_address) break;
        a = disassemble(d, a);
    }

    // The bar isn't on this view, so reset address
    if (row == m_height - 1)
    {
        setView(m_address);
        m_topAddress = m_address;
    }
    else while (row > m_height / 2)
    {
        // We need to adjust the bar
        m_topAddress = disassemble(d, m_topAddress);
        if (index == m_viewedAddresses.size())
        {
            m_viewedAddresses.push_back(a);
        }
        ++index;

        a = disassemble(d, a);
        --row;
    }
}

int DisassemblyWindow::findViewAddress(u16 address)
{
    auto it = std::find(m_viewedAddresses.begin(), m_viewedAddresses.end(), address);
    return (it == m_viewedAddresses.end()) ? -1 : (int)(it - m_viewedAddresses.begin());
}

void DisassemblyWindow::setView(u16 newTopAddress)
{
    if (-1 == findViewAddress(newTopAddress))
    {
        m_viewedAddresses.clear();
    }
    m_topAddress = newTopAddress;
}

void DisassemblyWindow::cursorDown()
{
    int index = findViewAddress(m_address);
    assert(index != -1);

    Disassembler d;
    u16 nextAddress = disassemble(d, m_address);

    if (index == m_viewedAddresses.size() - 1)
    {
        // This is the last known address of the viewed addresses.  Add the new one to the end
        m_viewedAddresses.push_back(nextAddress);
    }
    else
    {
        // Check that the next address is what is expect, otherwise the following address are invalid.
        if (m_viewedAddresses[index + 1] != nextAddress)
        {
            m_viewedAddresses.erase(m_viewedAddresses.begin() + index + 1, m_viewedAddresses.end());
            m_viewedAddresses.push_back(nextAddress);
        }
    }

    m_address = nextAddress;
}

void DisassemblyWindow::cursorUp()
{
    int index = findViewAddress(m_address);

    if (index == 0 || index == -1)
    {
        // We don't know the previous address.  Keep going back one byte until the disassembly of the instruction
        // finished up on the current address.
        u16 prevAddress = backInstruction(m_address);
        m_viewedAddresses.insert(m_viewedAddresses.begin(), prevAddress);
        m_address = prevAddress;
    }
    else
    {
        m_address = m_viewedAddresses[index - 1];
    }
}

void DisassemblyWindow::setCursor(u16 address)
{
    m_address = address;
    adjustBar();
}

void DisassemblyWindow::onDraw(Draw& draw)
{
    Disassembler d;
    u16 a = m_topAddress;
    u8 selectColour = draw.attr(Colour::Black, Colour::Yellow, true);
    u8 breakpointColour = draw.attr(Colour::Yellow, Colour::Red, true);
    u8 pcColour = draw.attr(Colour::White, Colour::Green, true);

    u8 bkg2 = m_bkgColour & ~0x40;
    for (int row = 1; row < m_height - 1; ++row)
    {
        u16 next = disassemble(d, a);
        u8 colour = (a == m_address)
            ? selectColour
            : (a == m_speccy.getZ80().PC())
                ? pcColour
                : m_speccy.hasUserBreakpointAt(a)
                    ? breakpointColour
                    : row & 1
                        ? m_bkgColour
                        : bkg2;

        draw.attrRect(m_x, m_y + row, m_width, 1, colour);
        draw.printString(m_x + 2, m_y + row, d.addressAndBytes(a).c_str(), colour);
        draw.printString(m_x + 21, m_y + row, d.opCode().c_str(), colour);
        draw.printString(m_x + 26, m_y + row, d.operands().c_str(), colour);

        if (m_speccy.hasUserBreakpointAt(a))
        {
            draw.printChar(m_x + 1, m_y + row, ')', colour, gGfxFont);
        }
        a = next;
    }

}

void DisassemblyWindow::onKey(sf::Keyboard::Key key)
{
    using K = sf::Keyboard::Key;
    switch (key)
    {
    case K::Up:
        cursorUp();
        adjustBar();
        break;

    case K::Down:
        cursorDown();
        adjustBar();
        break;

    case K::PageUp:
        for (int i = 0; i < (m_height - 2); ++i)
        {
            cursorUp();
        }
        adjustBar();
        break;

    case K::PageDown:
        for (int i = 0; i < (m_height - 2); ++i)
        {
            cursorDown();
        }
        adjustBar();
        break;

    case K::F9:
        m_speccy.toggleBreakpoint(m_address);
        break;

    default:
        break;
    }
}

u16 DisassemblyWindow::backInstruction(u16 address)
{
    u16 count = 1;
    Disassembler d;

    // Keep disassembling back until the address after the instruction is equal
    // to the current one
    while (count < 4) {
        u16 a = address - count;
        a = disassemble(d, a);
        if (a == address)
        {
            // This instruction will do!
            return address - count;
        }
        ++count;
    }

    // Couldn't find a suitable instruction, fallback: just go back one byte
    return address - 1;
}

u16 DisassemblyWindow::disassemble(Disassembler& d, u16 address)
{
    return d.disassemble(address,
        m_speccy.peek(address + 0),
        m_speccy.peek(address + 1),
        m_speccy.peek(address + 2),
        m_speccy.peek(address + 3));
}
