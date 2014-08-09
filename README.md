# Steam - Link Filter Disabler

This is a small program that aims to disable Steam's 'malicious' link checks. 
This includes both the in-client check (which causes delays in opening links), and the browser link screen. 

----------

## Usage

You can find compiled and packaged binaries (along with debug symbols) in the [Releases][1] section. 
The only requirement to running the pre-built binaries is having the [Microsoft Visual C++ 2010 Redistributable package][2] installed.

To use the app, simply run it as an administrator and wait while it patches Steam.
You have to be logged in in order for this to work, since the target module (friendsui.dll) is only loaded after a successful login.

You will have to re-patch Steam every time you launch it, since this is a simple in-memory patch.
After Steam is patched, you should be able to click on any link sent on chat and have it open instantly!

The following launch flags are currently available:

| Flag          | Description                                                                                      |
| ------------- | ------------------------------------------------------------------------------------------------ |
| **--retry**   | Forces the app to automatically retry patching upon failure.                                     |
| **--silent**  | Launches the app in silent mode, disabling the requirement for user input (also forces --retry). |
| **--ghost**   | Launches the app in ghost mode, disabling the console window (also forces --silent).             |

## Building

Visual Studio 2013 project files are included. 

The project is currently set to build with the **MSVC100** compiler (VS 2010) for compatibility reasons.
However, any compiler should work.

To build, simply open **LinkFilterDisabler.sln** in Visual Studio and build.
No external dependencies are needed.

## Disclaimer 

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This software is in no way sponsored, endorsed, or supported by, or associated with, Valve, Steam, or any of their partners.

**USE THIS AT YOUR OWN RISK!**

## Contributors

 - [Kevin Duchassin][3]


  [1]: https://github.com/OrfeasZ/SteamLinkFilterDisabler/releases
  [2]: http://www.microsoft.com/en-us/download/details.aspx?id=5555
  [3]: https://github.com/Efuveo