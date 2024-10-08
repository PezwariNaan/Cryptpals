#include "utility.hpp"
#include "encoding.hpp"
#include "encrypting.hpp"

int get_hamming_distance(std::vector<uint8_t> &first, std::vector<uint8_t> &second) {

    if (first.size() != second.size()) {
        std::cout << "Cannot find hamming distance. Strings are different lengths" << std::endl;
        return -1;
    }

    int score = 0;
    for (size_t i = 0; i < first.size(); i++) {
        uint8_t xored_byte = first[i] ^ second[i];
        while (xored_byte > 0) {
            score += xored_byte & 1;
            xored_byte >>= 1;
        }
    }

    return score;
}

int test(std::vector<uint8_t> first, std::vector<uint8_t> second) {
    int hamming_distance = get_hamming_distance(first, second);
    
    switch(hamming_distance) {
        case 37:
            std::cout << "Hamming Distance Check Passed.\n";
            break;
        case -1:
            return -1;
            break;
        default:
            std::cout << "Incorrect Hamming Distance\n";
            std::cout << "Result: " << hamming_distance << "\n";
            std::cout << "Goal : " << 37 << std::endl;
            return -1;
    }

    return 0;
}

int main(void) {
    std::string first  = "this is a test";
    std::string second = "wokka wokka!!!";
    std::vector<uint8_t> first_bytes(first.begin(), first.end());
    std::vector<uint8_t> second_bytes(second.begin(), second.end());

    if (test(first_bytes, second_bytes) < 0) return 1;

    std::string filename = "./Texts/challenge6.txt";
    std::string cipher_text = read_file(filename);
    std::vector<uint8_t> cipher_bytes = cp::base64_decode(cipher_text);

    int keysize = 2;
    const int MAX_KEYSIZE = 40;

    std::map<float, int> keysize_scores; // score, keysize

    // Get normalised edit distance between first 4 blocks of KEYSIZE length

    for (; keysize < MAX_KEYSIZE; keysize++) {
        std::vector<uint8_t> first_block;
        std::vector<uint8_t> second_block;
        std::vector<uint8_t> third_block;
        std::vector<uint8_t> fourth_block;

        for (int i = 0; i < keysize; i++) {
            first_block.push_back (cipher_bytes[i]);
            second_block.push_back(cipher_bytes[i + (keysize * 1)]);
            third_block.push_back (cipher_bytes[i + (keysize * 2)]);
            fourth_block.push_back(cipher_bytes[i + (keysize * 3)]);
        }

        float normalised_hamming_distance1 = get_hamming_distance(first_block, second_block) / keysize;
        float normalised_hamming_distance2 = get_hamming_distance(second_block, third_block)  / keysize;
        float normalised_hamming_distance3 = get_hamming_distance(third_block, fourth_block) / keysize;
        float normalised_hamming_distance4 = get_hamming_distance(second_block, third_block) / keysize;
        float normalised_hamming_distance5 = get_hamming_distance(second_block, fourth_block)/ keysize;
        float normalised_hamming_distance6 = get_hamming_distance(third_block, fourth_block) / keysize;

        float average_hamming_distance = (normalised_hamming_distance1  + \
                                          normalised_hamming_distance2  + \
                                          normalised_hamming_distance3  + \
                                          normalised_hamming_distance4  + \
                                          normalised_hamming_distance5  + \
                                          normalised_hamming_distance6) / 6.0f;
                                        
        keysize_scores.insert({average_hamming_distance, keysize});
    }    
    
    // Select keysize with lowest edit distance 

    int likely_keysize = keysize_scores.begin()->second;
    std::cout << "Keysize: " << likely_keysize << "\n";

    // Break cipher_bytes into keysize length blocks 
    std::map<int, std::vector<uint8_t>> keysize_blocks;
    int count = 1;
    
    // Transpose the blocks as they are populated 
    for (int j = 0; j < likely_keysize; j++) {
        std::vector<uint8_t> block;
        for (size_t i = 0; i < cipher_bytes.size(); i += likely_keysize) {
            block.push_back(cipher_bytes[i + j]);        
        }
        keysize_blocks.insert({count, block});
        count++;
    }

    auto element = keysize_blocks.begin();
    auto end = keysize_blocks.end();
    
    // Run attack_single_byte_xor on each transposed block 
    std::vector<result> result_vector;
    for (;element != end; element++) {
        result results = cp::attack_single_byte_xor(element->second);
        result_vector.push_back(results);
    }

    for (size_t i = 0; i < result_vector.size(); i++) {
        std::cout << result_vector[i].key;
    }

    std::cout << std::endl; 

    return 0;
}