// CDDA_Map.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <vector>
#include <stdlib.h>

struct color {
    int r;
    int g;
    int b;
};
struct rule {
    color c;
    char sym;
};

std::vector<rule> parse_rules() {

    std::vector<rule> rules;
    std::ifstream file( "rules.txt" );
    std::string line;
    if( file.is_open() ) {
        while( !file.eof() ) {
            line.clear();
            std::getline( file, line );

            char* pEnd;
            int b = std::strtol( line.c_str(), &pEnd, 10 );
            int g = std::strtol( pEnd, &pEnd, 10 );
            int r = std::strtol( pEnd, &pEnd, 10 );
            char sym = pEnd[1];
            rules.push_back( rule{ color{ r, g, b }, sym } );
        }
    }
    return rules;
}

char checkRules( std::vector<rule> rules, color c ){

    for( int i = 0; i < rules.size(); i++ ) {

        if( rules[i].c.r == c.r &&
            rules[i].c.g == c.g &&
            rules[i].c.b == c.b ) {
            return rules[i].sym;
        }
    }
    //default to space
    return ' ';
}

unsigned int toInt( std::vector<char> bytes ) {
    unsigned int value = 0;

    for( int i = 0; i < bytes.size(); i++ ) {
        if( i != 0 ) {
            value += ( bytes[i] * ( 256 * ( i ) ) );
        } else {
            value += ( bytes[i] );
        }

    }
    return value;
}

void translate( std::vector<char> data ) {
    for( int i = 0; i < data.size(); i++ ) {
        std::bitset<8> b( data[i] );
        std::cout << b << " ";
    }
    std::cout << '\n';
}

std::vector<char> readImage( const char* filename ) {
    std::vector<char> data;

    std::ifstream file( filename, std::ios::in | std::ios::binary | std::ios::ate );
    if( file.is_open() ) {
        file.seekg( 0, std::ios::end );
        int size = file.tellg();
        file.seekg( 0, std::ios::beg );

        std::vector<char> data( size );
        file.read( (char*)&data[0], size );

        file.close();

        //translate( data );

        return data;

    } else std::cout << "ERR: Unable to open file";

    return data;
}

int main()
{
    const int HEADER_SIZE = 14;

    std::vector<rule> rules = parse_rules();
    std::cout << "Valid Rules: " << rules.size() << '\n';

    std::string filename;

    std::cout << "Filename: ";
    std::getline( std::cin, filename );

    std::cout << "Loading Image...";
    std::vector<char> data = readImage( (filename + ".bmp").c_str() );
    std::cout << "Done\n";

    //Header
    std::cout << "Parsing Header:";
    std::vector<char> header;
    for( int i = 0; i < HEADER_SIZE; i++ ) {
        header.push_back( data[i] );
    }

    for( int i = 0; i < HEADER_SIZE; i++ ) {
        data.erase( data.begin() );
    }

    std::cout << '\n';

    //Infoheader
    std::vector<char> sizebytes;
    for( int i = 0; i < 4; i++ ) {
        sizebytes.push_back(data[i]);
    }
    int infosize = toInt(sizebytes);

    std::cout << "Infoheader size: " << infosize << '\n';

    std::vector<char> info;
    for( int i = 0; i < infosize; i++ ) {
        info.push_back( data[i] );
    }

    translate( info );

    const int WIDTH_OFFSET = 4;
    const int HEIGHT_OFFSET = 8;
    const int BITDEPTH_OFFSET = 14;
    std::cout << ". Getting Width...";
    std::vector<char> widthbytes;
    for( int i = 0; i < 4; i++) {
        widthbytes.push_back( data[i + WIDTH_OFFSET] );
    }
    std::cout << "Done\n";

    std::cout << ". Getting Height...";
    std::vector<char> heightbytes;
    for( int i = 0; i < 4; i++ ) {
        heightbytes.push_back( data[i + HEIGHT_OFFSET] );
    }
    std::cout << "Done\n";

    std::cout << ". Getting Bitdepth...";
    std::vector<char> bitbytes;
    for( int i = 0; i < 2; i++ ) {
        bitbytes.push_back( data[i + BITDEPTH_OFFSET] );
    }
    std::cout << "Done\n";

    int width = (unsigned char)toInt( widthbytes );
    int height = toInt( heightbytes );
    int bitdepth = toInt( bitbytes );
    int chardepth = bitdepth / 8;
    int blocksize = width * height * chardepth;

    std::cout << "Width: " << width << '\n';
    std::cout << "Height: " << height << '\n';
    std::cout << "Bit Depth: " << bitdepth << '\n';
    std::cout << "Char Depth: " << chardepth << '\n';
    std::cout << "Pixels: " << blocksize << '\n';

    for( int i = 0; i < infosize; i++ ) {
        data.erase( data.begin() );
    }
    std::cout << "Done\n";

    if( bitdepth != 24 ) {
        std::cout << "ERR: Bitmap must be 24-bit";
    }

    std::vector<color> colors;

    for( int i = 0; i < blocksize; i++ ) {
        unsigned char r = data[i];
        unsigned char g = data[++i];
        unsigned char b = data[++i];

        colors.push_back( color{ (int)r, (int)g, (int)b } );
    }

    std::vector<char> out;

    for( int i = 0; i < colors.size(); i++ ) {
        out.push_back( checkRules( rules, colors[i] ) );
    }

    std::ofstream outfile;
    outfile.open( (filename + ".txt").c_str() );
    for( int i = 0; i < height; i++ ) {
        outfile << "\"";
        for( int j = 0; j < width; j++ ) {
            outfile << out[j + ( width * i )];
        }
        outfile << "\"";
        if( i+1 < height ) {
            outfile << ",";
        }
        outfile << '\n';
    }
    outfile << '\n';

    int mapwidth = width / 24;
    int mapheight = height / 24;

    outfile << "\"om_terrain\": [\n";

    for( int i = 0; i < mapheight; i++ ) {
        outfile << "  [ ";
        for( int j = 0; j < mapwidth; j++ ) {
            outfile << "\"" << filename + "_" + std::to_string( i ) + "_" + std::to_string( j ) << "\"";
            if( j + 1 < mapwidth ) {
                outfile << ",";
            } else {
                if( i + 1 < mapheight ) {
                    outfile << " ],\n";
                } else {
                    outfile << " ]\n";
                }
                
            }
        }
    }
    outfile << "],";

    outfile << " \n";

    //{ "point": [ 0, 0, 0 ], "overmap": "municipal_reactor_north" },
    for( int i = 0; i < mapheight; i++ ) {
        for( int j = 0; j < mapwidth; j++ ) {
            outfile << "{ \"point\": [ " << j << ", " << i << ", " << "9 ], \"overmap\": \"" << filename + "_" + std::to_string( i ) + "_" + std::to_string( j ) << "_north\" }";
            if( i + 1 < mapheight || j + 1 < mapwidth ) {
                outfile << ",\n";
            }
            else {
                outfile << "\n";
            }
        }
    }


    outfile.close();

    std::cout << "Saved to: " << filename << ".txt\n";


    std::cout << '\n';
    
}