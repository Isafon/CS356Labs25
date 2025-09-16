// Author: Isa Fontana 835760710
// Author's Note: I created an entire readme file because habit (ECE student), feels wrong to submit without one. (TAs lmk if you want it)
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using std::size_t;
static void printAndExit(const std::string &msg)
{
    // printing required error messages ONLY
    std::cerr << msg << std::endl;
    std::exit(1);
}
static bool fileExists(const std::string &path)
{
    std::ifstream f(path, std::ios::binary);
    return static_cast<bool>(f);
}
static std::string slurp(const std::string &path, const char which)
{
    // I = inp, K = key (for error text)
    std::ifstream f(path, std::ios::binary);
    if (!f)
    {
        if (which == 'I')
            printAndExit("Input File Does Not Exist");
        else
            printAndExit("Key File Does Not Exist");
    }
    return std::string((std::istreambuf_iterator<char>(f)),std::istreambuf_iterator<char>());
}
static void dump(const std::string &path, const std::string &data)
{
    std::ofstream f(path, std::ios::binary);
    f.write(data.data(), static_cast<std::streamsize>(data.size()));
}
// ISA'S HELPER METHODS
static inline void padTo16(std::vector<uint8_t> &blk)
{
    while (blk.size() < 16)
        blk.push_back(0x81);
}
static inline void xorWithKey16(std::vector<uint8_t> &blk, const std::string &key)
{
    // key assumed to be AT LEAST 16 bytes (Given in ASN)
    for (int i = 0; i < 16; ++i)
        blk[i] ^= static_cast<uint8_t>(key[i]);
}
// encrypt-dir byte swap (after XOR)
static void swapBytesEnc(std::vector<uint8_t> &blk, const std::string &key)
{
    int start = 0;
    int end = 15;
    size_t k = 0;
    while (start < end)
    {
        const uint8_t kv = static_cast<uint8_t>(key[k]);
        if ((kv % 2) == 1)
        {
            std::swap(blk[start], blk[end]);
            ++start;
            --end;
        }
        else
        {
            ++start;
        }
        k = (k + 1) % key.size();
    }
}
// decrypt-dir byte swap (inverse of above, before XOR)
static void swapBytesDec(std::vector<uint8_t> &blk, const std::string &key)
{
    //note swap positions during encryption & do it again in reverse
    int s = 0, e = 15;
    size_t k = 0;
    std::vector<std::pair<int, int>> performed;
    performed.reserve(8);
    while (s < e)
    {
        const uint8_t kv = static_cast<uint8_t>(key[k]);
        if ((kv % 2) == 1)
        {
            performed.emplace_back(s, e);
            ++s;
            --e;
        }
        else
        {
            ++s;
        }
        k = (k + 1) % key.size();
    }
    for (int i = static_cast<int>(performed.size()) - 1; i >= 0; --i)
    {
        int a = performed[i].first;
        int b = performed[i].second;
        std::swap(blk[a], blk[b]);
    }
}
static void encryptBlockAppend(const std::vector<uint8_t> &inBlk, const std::string &key, std::string &out)
{
    std::vector<uint8_t> blk = inBlk;
    padTo16(blk);
    xorWithKey16(blk, key);
    swapBytesEnc(blk, key);
    out.append(reinterpret_cast<const char *>(blk.data()), 16);
}
static void decryptBlockAppend(const std::vector<uint8_t> &inBlk, const std::string &key, std::string &out)
{
    std::vector<uint8_t> blk = inBlk;
    // pad cipher if less than 16 bytes
    padTo16(blk);
    swapBytesDec(blk, key);
    xorWithKey16(blk, key);
    out.append(reinterpret_cast<const char *>(blk.data()), 16);
}
// stream cipher XOR function
static std::string streamXor(const std::string &data, const std::string &key)
{
    if (key.empty())
        return data;
    std::string out = data;
    const size_t K = key.size();
    for (size_t i = 0; i < data.size(); ++i)
    {
        out[i] = static_cast<char>(static_cast<uint8_t>(data[i]) ^ static_cast<uint8_t>(key[i % K]));
    }
    return out;
}
// main
int main(int argc, char *argv[])
{
    // exactly 5 args required --> if not, exit(1) silently (lab ASN has no message for this case)
    if (argc != 6)
        return 1;

    const std::string type = argv[1]; // B or S
    const std::string inP = argv[2];
    const std::string outP = argv[3];
    const std::string keyP = argv[4];
    const std::string mode = argv[5]; // E or D

    if (type != "B" && type != "S")
        printAndExit("Invalid Function Type");
    if (mode != "E" && mode != "D")
        printAndExit("Invalid Mode Type");
    if (!fileExists(inP))
        printAndExit("Input File Does Not Exist");
    if (!fileExists(keyP))
        printAndExit("Key File Does Not Exist");

    const std::string input = slurp(inP, 'I');
    const std::string key = slurp(keyP, 'K');
    std::string result;
    if (type == "S")
    {
        // stream cipher, E & D func
        result = streamXor(input, key);
    }
    else
    {
        /* block cipher: 16-byte blocks, pad on E... on D: swap -> XOR -> depad
        ASN says, key file for block cipher is 16 bytes, assume grader provides valid key...*/
        const size_t N = input.size();
        size_t i = 0;
        if (mode == "E")
        {
            while (i < N)
            {
                std::vector<uint8_t> blk;
                blk.reserve(16);
                for (int j = 0; j < 16 && i < N; ++j, ++i)
                {
                    blk.push_back(static_cast<uint8_t>(input[i]));
                }
                encryptBlockAppend(blk, key, result);
            }
        }
        else
        { // mode == D (side eye)
            while (i < N)
            {
                std::vector<uint8_t> blk;
                blk.reserve(16);
                for (int j = 0; j < 16 && i < N; ++j, ++i)
                {
                    blk.push_back(static_cast<uint8_t>(input[i]));
                }
                decryptBlockAppend(blk, key, result);
            }
            // remove padding from the VERY END only (plaintext was ASCII 0–127)
            while (!result.empty() && static_cast<uint8_t>(result.back()) == 0x81)
            {
                result.pop_back();
            }
        }
    }
    dump(outP, result);
    return 0;
}
// ate that.