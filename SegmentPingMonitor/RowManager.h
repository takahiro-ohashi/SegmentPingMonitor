#pragma once
#include <afxmt.h> // 必要なヘッダーファイルをインクルード
#include <vector>
#include <string>
#include <regex>

class RowManager {
    struct S {
        char ipaddr[512];
        int v;
    };
public:
    explicit RowManager(const std::vector<int>& row)
        : rowAddresses_(row), currentIndex_(0) {
    }

    int GetNextRow() {
        CSingleLock lock(&criticalSection_, TRUE); // クリティカルセクションにロックをかける

        if (currentIndex_ >= rowAddresses_.size()) {
            return -1;  // rowが残っていない場合は-1を返す
        }

        int row = rowAddresses_[currentIndex_];
        currentIndex_++;

        return row;
    }
    void SetIpAddr(char* ipaddr) {
        S ip;

        ip.v=validateAddress(ipaddr);
        sprintf_s(ip.ipaddr, 512, "%s", ipaddr);
        ipAddress_.push_back(ip);
    }
    char* GetIpAddr(int Row) {
        return ipAddress_[Row].ipaddr;
    }
    int GetIPV(int Row) {
        return ipAddress_[Row].v;
    }
    void del() {
        rowAddresses_.~vector();
        ipAddress_.~vector();
    }
    int validateAddress(TCHAR* address)
    {
        // Regex expression for validating IPv4
        std::regex ipv4("(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])");

        // Regex expression for validating IPv6
        std::regex ipv6("((([0-9a-fA-F]){1,4})\\:){7}([0-9a-fA-F]){1,4}");

        if (!((regex_match(address, ipv4) ^ regex_match(address, ipv6))))
        {
            return -1;
        }
        // Checking if it is a valid IPv4 addresses
        if (regex_match(address, ipv4))
        {
            return 1;
        }
        // Checking if it is a valid IPv6 addresses
        //else if (regex_match(address, ipv6))
        return 2;

        // Return Invalid
    }
private:
    std::vector<int> rowAddresses_;
    std::vector<S> ipAddress_;
    size_t currentIndex_;
    CCriticalSection criticalSection_; // MFCのクリティカルセクション
};

