#include <tinyxml2.h>
#include <new>

namespace android {

extern "C" void _ZN8tinyxml211XMLDocumentC1Eb(tinyxml2::XMLDocument* thisptr,
                                              bool processEntities) {
    new (thisptr) tinyxml2::XMLDocument(processEntities);
}

}  // namespace android
