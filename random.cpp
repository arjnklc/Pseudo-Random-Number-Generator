#include <iostream>
#include <tomcrypt.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;


// Returns SHA256 of a given string
string getSHA256(const string &input) {
    unsigned char* hashResult = new unsigned char[sha256_desc.hashsize];
    hash_state md;
    sha256_init(&md);
    sha256_process(&md, (const unsigned char*) input.c_str(), input.size());
    sha256_done(&md, hashResult);
    string result = "";
    stringstream ss;
    for(int i = 0; i < sha256_desc.hashsize; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hashResult[i];
    }
    return ss.str();
}

// Get random numbers from /dev/urandom
unsigned int get_dev_urandom() { 
    ifstream f("/dev/urandom"); 
    unsigned int seed; 
    f.read(reinterpret_cast<char*>(&seed), sizeof(seed)); 
    return seed;
} 

// Get clock cycles since processor was powered on
unsigned long long get_rdtsc() {
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long long)hi << 32) | lo;
}

string getSeed() {
    string rdtsc = to_string(get_rdtsc());
    string urandom = to_string(get_dev_urandom());
    string rdtsc_hash = getSHA256(rdtsc);
    string urandom_hash = getSHA256(rdtsc);
    string result = getSHA256(rdtsc_hash + urandom_hash);
    return result;
}


// It uses Yarrow from libtomcrypt library to generate bytes
unsigned char* generateRandomBytes(int bits) {
    bits = bits / 8; // 8 bytes = 64 bit
    string seed = getSeed();
    prng_state prng;
    unsigned char buf[8];  // 8 char = 64 bit
    
    int err;

    /* start it */
    if ((err = yarrow_start(&prng)) != CRYPT_OK) {
        printf("Start error: %s\n", error_to_string(err));
    }

    /* add entropy */
    if ((err = yarrow_add_entropy( (unsigned char*) seed.c_str(), seed.length(), &prng)) != CRYPT_OK) {
        printf("Add_entropy error: %s\n", error_to_string(err));
    }

    /* ready and read */
    if ((err = yarrow_ready(&prng)) != CRYPT_OK) {
        printf("Ready error: %s\n", error_to_string(err));
    }
    yarrow_read(buf, sizeof(buf), &prng);
    
    // +1 for null terminator
    unsigned char* buff = new unsigned char[bits + 1];
    
    for (int i = 0; i < bits; i++) {
        buff[i] = buf[i];
    }

    buff[bits] = '\0';
    return buff;
}


// Basically converts byte array to a long long integer
unsigned long long generateRandomInt(int bits) {
    unsigned char *buf = generateRandomBytes(bits);
    unsigned long long random = 0;
    for (int i = 0; i < bits / 8; ++i) {
        random = random | ((unsigned long long) buf[i] << ((bits / 8) * i));
    }
    delete buf;
    return random;
}


// Generates an integer with an upper limit
unsigned long long generateRandomInt(int bits, long upperLimit) {
    unsigned long long int random = generateRandomInt(bits);
    return random % upperLimit;
}


// Writes a string to a file
void writeTextToFile(string filename, string output) {
    ofstream myfile(filename.c_str(), ios::app);
    if (myfile.is_open()) {
        myfile << output << endl;
        myfile.close();
    }
}


int main() {

    const char* filename = "output.txt";   // Output file for generated numbers
    
    const int BITS  = 64;    // BITS of numbers
    const int COUNT = 1;  // Counts of number that will be generated.
    
    for (int i = 0; i < COUNT; i++) {
        unsigned long long randomNumber = generateRandomInt(BITS);
        writeTextToFile(filename, to_string(randomNumber) );
    }

	return 0;
}
