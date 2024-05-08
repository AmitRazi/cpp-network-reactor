#include "st_reactor.hpp"

int main() {
    void *reacter = createReactor();
    startReactor(reacter);
    return 0;
}
