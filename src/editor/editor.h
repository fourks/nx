//----------------------------------------------------------------------------------------------------------------------
//! Defines the Editor UI that manages a selected EditorData
//!
//! @author     Matt Davies
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include <core.h>
#include <editor/data.h>

//----------------------------------------------------------------------------------------------------------------------
//! Represents an editor view in the UI

class EditorData;
class Draw;
struct KeyEvent;

class Editor
{
public:
    Editor();

    struct State
    {
        int             x;          //!< X coordinate of top-left corner of window.
        int             y;          //!< Y coordinate of top-left corner of window.
        int             width;      //!< Width of editor area in cells.
        int             height;     //!< Height of editor area in cells.
        u8              colour;     //!< Base colour.
        u8              cursor;     //!< Cursor colour.
    };

    //! Apply the state to the editor
    void apply(const State& state);

    //! Draw the editor
    void render(Draw& draw);

    //! Called by the overlay to handle key presses.
    bool key(const KeyEvent& kev);

    //! Called by the overlay to handle textual input.
    void text(char ch);

    //! Return the state.
    State& getState() { return m_state; }

    //! Return the state.
    const State& getState() const { return m_state; }

    //! Set the current editor attached to this UI.
    void setData(EditorData& editorData);

    //
    // Cursor control
    //

    //! Set cursor to top of document.
    void gotoTop();

    //! Set cursor to bottom of document.
    void gotoBottom();

    //! Set cursor to beginning of line.
    void home();

    //! Set cursor to end of line.
    void end();

private:
    State               m_state;
    EditorData*         m_data;
    Pos                 m_cursor;
    Pos                 m_top;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
