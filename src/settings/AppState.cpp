
#include "polyhydra/settings/AppState.h"

namespace polyhydra::Internal
{
AppState::AppSettings AppState::settings{};
AppState::AppCallbacks AppState::callbacks{};
AppState::RenderStatistics AppState::statistics{};
} // namespace polyhydra::Internal