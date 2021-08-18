// Example program
#include <iostream>
#include <string>


#define MAC_ADDRESS_STRING_LENGTH 18 // accounting for the null char
#define MAC_ADDRESS_LENGTH 6
#define WHITELIST_LENGTH 1024

uint8_t hexCharToUint8t(char c) {
    if(c == '0'){
        return 0;
    }
    else if(c == '1'){
        return 1;
    }
    else if(c == '2'){
        return 2;
    }
    else if(c == '3'){
        return 3;
    }
    else if(c == '4'){
        return 4;
    }
    else if(c == '5'){
        return 5;
    }
    else if(c == '6'){
        return 6;
    }
    else if(c == '7'){
        return 7;
    }
    else if(c == '8'){
        return 8;
    }
    else if(c == '9'){
        return 9;
    }
    else if(c == 'a'){
        return 10;
    }
    else if(c == 'b'){
        return 11;
    }
    else if(c == 'c'){
        return 12;
    }
    else if(c == 'd'){
        return 13;
    }
    else if(c == 'e'){
        return 14;
    }
    else if(c == 'f'){
        return 15;
    } else {
        printf("Update whiteList ERROR: invalid character in hex\n");
        return 0;
    }

}


int main()
{
    uint8_t whiteList[WHITELIST_LENGTH][MAC_ADDRESS_LENGTH];
    int whiteListCount = 0;
    char* whiteListString = "aabbccddeeff112233445566eeffbbccaadd";
    uint8_t validMac = 1;
    for(int i = 0; i < WHITELIST_LENGTH; i++) {
        uint8_t outMac[MAC_ADDRESS_LENGTH];
        if(whiteListString[i*MAC_ADDRESS_LENGTH*2] == '\0'){
            // End of the string, return
            break;
        }
        for(int j = 0; j < MAC_ADDRESS_LENGTH; j++){
            char c1 = whiteListString[i*MAC_ADDRESS_LENGTH*2 + j*2];
            char c2 = whiteListString[i*MAC_ADDRESS_LENGTH*2 + j*2 + 1];
            if(c1 == '\0' || c2 == '\0'){
                printf("Update whiteList ERROR: whiteListString truncated\n");
                validMac = 0;
                break;
            }
          outMac[j] = hexCharToUint8t(c1) * 16 + hexCharToUint8t(c2);
        }
        
        if(!validMac) {
            break;
        }
        for(int k = 0; k < MAC_ADDRESS_LENGTH; k++){
            whiteList[i][k] = outMac[k];
        }
        whiteListCount = whiteListCount + 1;
        
       
    }

    // Put 0's in all next white listed MAC addresses
    for(int i = whiteListCount; i < WHITELIST_LENGTH; i++) {
        for(int k = 0; k < MAC_ADDRESS_LENGTH; k++){
            whiteList[i][k] = 0;
        }
    }
    // for(int i = 0; i < WHITELIST_LENGTH; i++) {
    //     printf("%02x:%02x:%02x:%02x:%02x:%02x\n", whiteList[i][0], whiteList[i][1], whiteList[i][2], whiteList[i][3], whiteList[i][4], whiteList[i][5]);
    // }
    return 0;
}


