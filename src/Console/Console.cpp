#include "Console.hpp"

#include "../OpenGLHeaders.hpp"

#include "../GLSL/Program.hpp"
#include "../Graphical/GL/Rectangle.hpp"
#include "../Graphical/Text.hpp"
#include "../Graphical/Texture.hpp"
#include "../Input/Event.hpp"
#include "../Input/Input.hpp"
#include "../Math/Math.hpp"
#include "../State/State.hpp"
#include "../Utils/Asset.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

Console::Console(Asset* asset) {
  vec2 res      = asset->cfg()->graphics.res;
  vec2 textPos  = vec2(10, res.y / 2 - 30);
  vec2 errorPos = vec2(10, res.y / 2);

  mShowAutoComplete = false;
  mAsset            = asset;
  mCurrentText      = "";
  mBoundingBox      = Rect(0, 0, res.x, res.y / 2);
  mRect             = new GL::Rectangle(mBoundingBox);
  mAutoCompleteBox  = new GL::Rectangle(Rect(0, 0, 0, 0));
  mText             = new Text(mFont, "> _", textPos, 20, Text::WHITE);
  mError            = new Text(mFont, "Error:", errorPos, 20, Text::RED);
  mErrorDisplayed   = 0;

  mError->isVisible(false);

  // Specific program for the console since the console
  // is just drawn in black with alpha
  mProgram = new Program("shaders/GUI/ColorRect.vsfs", 0);
  mProgram->bindAttribs({ "position", "texcoord" }, { 0, 1 });
  mProgram->link();
  mProgram->setUniform("screenRes", res, "guiOffset", vec2());
  mProgram->setUniform("guiColor", vec4(0, 0, 0, 0.9));

  // Add default commands
  addCommand("game.exit", [](Asset*,
                             const Input::Event& event,
                             const std::string& input) {
    event.sendStateChange(States::QuitAll);
    return "";
  });
}

Console::~Console() {
  delete mProgram;
  delete mRect;
  delete mText;
  delete mError;
  delete mAutoCompleteBox;

  for (auto a : mAutoComplete)
    delete a;

  for (auto h : mHistory)
    delete h;

  mAutoComplete.clear();
  mHistory.clear();
  mCommands.clear();
}

/**
 * @brief
 *   This function adds a new command that can be invoked from
 *   the console using the given name.
 *
 *   All names should have a namespace followed by a `.`. Example:
 *   config.res
 *
 *   The return value of the Console::Handler function is a string,
 *   which when not empty, will print an error onto the screen.
 *
 * @param name
 *   Name of the function, is unique.
 *
 * @param h
 *   The handler itself.
 */
void Console::addCommand(const std::string& name, Console::Handler h) {
  if (name.find(".") == std::string::npos)
    throw new Error("Must have namespace in name. Ex: config.res");

  mCommands[name] = h;
  mAutoComplete.push_back(new Text(mFont, name, vec2(0, 0), 20, Text::WHITE));
}

/**
 * @brief
 *   Sets an error just below where the user is typing in a
 *   red text that is displayed for three seconds before
 *   being hidden again. Will always have the words "Error: " infront
 *   of it.
 *
 * @param message
 *   The message to display
 */
void Console::setError(const std::string& message) {
  mError->setText("Error: " + message);
  mError->isVisible(true);
  mErrorDisplayed = 3;
}

/**
 * @brief
 *   When a command has been executed, this function
 *   is called which adds the current text to a list of
 *   history as a new Text element.
 *
 *   This is displayed on the screen until it goes outside it.
 *   A text element will gradiually be moved upwards until
 *   it is outside of the screen, at which point it will be hidden
 */
void Console::addHistory() {
  vec2  pos = vec2(10, mAsset->cfg()->graphics.res.y / 2 - 60);
  Text* t   = new Text(mFont, mCurrentText, pos, 20, Text::WHITE);

  t->isVisible(true);

  int index = mHistory.size();
  for (auto a : mHistory) {
    vec2 newPos = vec2(0, -index * 30);

    if (newPos.y + pos.y < 0)
      a->isVisible(false);

    a->setOffset(newPos);
    index--;
  }

  log("Adding ", mCurrentText, " to history");

  mHistory.push_back(t);
}

/**
 * @brief
 *   Checks the current text for errors, if any, prints error.
 *   If there are no errors and the text matches a command
 *   it will call that command with the text within the brackets.
 *
 *   Good commands:
 *
 *     config.res(100, 100))
 *     config.res.x(100)
 *     config.res.y(100)
 *
 *   Bad commands:
 *     config.res = 100
 *     config.res(10
 *     config.res)(
 *
 * @param event
 */
void Console::doCommand(const Input::Event& event) {
  size_t startBracket = mCurrentText.find_first_of("(");
  size_t endBracket   = mCurrentText.find_last_of(")");

  if (startBracket == std::string::npos)
    return setError("Commands must use brackets");

  if (startBracket > endBracket)
    return setError("Bracket alignment incorrect");

  if (endBracket != mCurrentText.size() - 1)
    return setError("Commands must end with a closing bracket");

  std::string command    = mCurrentText.substr(0, startBracket);
  std::string parameters = mCurrentText.substr(startBracket + 1, endBracket);
  parameters.pop_back();

  if (mCommands.count(command) == 0)
    return setError("No such command '" + command + "'");

  std::string output = mCommands[command](mAsset, event, parameters);
  log("Console: ", command, " - Output: ", output);

  if (output != "")
    return setError(output);

  addHistory();
  setText("");
}

/**
 * @brief
 *   Sets the text of the console, also starts
 *   the autocomplete.
 *
 * @param s
 */
void Console::setText(const std::string& s) {
  mCurrentText = s;
  mText->setText("> " + mCurrentText + "_");
  setAutoComplete();
}

/**
 * @brief
 *   Checks the current text to see if there are any commands
 *   that contain the words in them. If so, display those
 *   commands.
 *
 *   Also adjusts the autocompletion box to match width
 *   and height of the texts.
 */
void Console::setAutoComplete() {
  if (mCommands.size() == 0 || mCurrentText.size() == 0) {
    mShowAutoComplete = false;
    return;
  }

  vec2 res      = mAsset->cfg()->graphics.res;
  Rect box      = Rect(5, res.y / 2 + 30, 0, 0);
  int  index    = 0;
  vec2 startPos = vec2(10, res.y / 2 + 30);

  for (auto a : mAutoComplete) {
    bool hasText = a->getText().find(mCurrentText) != std::string::npos;
    if (hasText) {
      a->isVisible(true);
      a->setOffset(startPos + vec2(0, 20 * index));
      box.size.x = max(a->size().x, box.size.x);
      box.size.y = max(a->size().y, box.size.y);
      index++;
    } else {
      a->isVisible(false);
    }
  }

  if (box.size.y > 0 && index != 0) {
    box.size.x        = box.size.x + 10;
    mShowAutoComplete = true;
    mAutoCompleteBox->change(box);
  } else {
    mShowAutoComplete = false;
  }
}

/**
 * @brief
 *   Handles the key input for a console
 *
 * @return
 */
void Console::input(const Input::Event& event) {
  bool isConsoleKey =
    mAsset->input()->checkKey(Input::Action::Console, event.key());

  if (!isVisible() && isConsoleKey) {
    mPrevInputOpened = true;
    isVisible(true);
    return event.stopPropgation();
  }

  if (!isVisible()) {
    return;
  }

  if (event.keyPressed(GLFW_KEY_ESCAPE)) {
    isVisible(false);
    return event.stopPropgation();
  }

  if (event.isKeyHeldDown(GLFW_KEY_BACKSPACE)) {
    if (mCurrentText.length() > 0)
      mCurrentText.pop_back();

    setText(mCurrentText);
    return event.stopPropgation();
  }

  if (event.keyPressed(GLFW_KEY_ENTER)) {
    doCommand(event);
    return event.stopPropgation();
  }

  if (event == Input::Event::Type::CharacterInput && !mPrevInputOpened) {
    setText(mCurrentText + event.character());
    return event.stopPropgation();
  } else if (event == Input::Event::Type::CharacterInput) {
    mPrevInputOpened = false;
  }
}

/**
 * @brief
 *   Updates the console. This only does one thing
 *   at the moment which is count down the time
 *   it should display the error, if it is displayed
 *
 * @param deltaTime
 */
void Console::update(float deltaTime) {
  if (mErrorDisplayed > 0)
    mErrorDisplayed = mErrorDisplayed - deltaTime;


  if (mErrorDisplayed <= 0 && mError->isVisible()) {
    mError->isVisible(false);
    mErrorDisplayed = 0;
  }
}

/**
 * @brief Draw!
 */
void Console::draw() {
  if (!isVisible())
    return;

  mProgram->bind();
  mRect->draw();

  if (mShowAutoComplete) {
    mAutoCompleteBox->draw();
    for (auto a : mAutoComplete)
      a->draw();
  }

  mText->draw();
  mError->draw();

  for (auto a : mHistory) {
    a->draw();
  }
}