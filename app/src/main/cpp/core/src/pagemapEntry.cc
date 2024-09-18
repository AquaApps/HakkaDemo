#include <bits/sysconf.h>
#include "hakka.h"

auto hakka::PageEntry::getPagemapEntry(handle pagemapFd,
                                          ptr_t vaddr) -> PageEntry { // NOLINT(*-easily-swappable-parameters)
    hakka::PageEntry entry{};
    size_t nread;
    int64_t ret;
    uint64_t data;
    ptr_t file_offset =
            (vaddr / sysconf(_SC_PAGE_SIZE)) * sizeof(data); // NOLINT(*-narrowing-conversions)
    struct iovec iov{&data, sizeof(data)};
    nread = 0;
    while (nread < sizeof(data)) {
        iov.iov_len = sizeof(data) - nread;
        ret = preadv(pagemapFd, &iov, 1, file_offset);
        nread += ret;
        if (ret <= 0) {
            return entry;
        }
    }
    entry.swapped = (data >> 62) & 1;
    entry.present = (data >> 63) & 1;
    return entry;
}