// CDDA_Map.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <vector>
#include <stdlib.h>

#define point std::pair<int, int>
#define rect std::pair<point, point>
#define mon_spawn std::pair<rect, mon_rule>
#define item_spawn std::pair<rect, item_rule>

const std::string TAB = "  ";
const int HEADER_SIZE = 14;

#define RIGHT point{ 1, 0 }
#define LEFT point{ -1, 0 }
#define UP point{ 0, 1 }
#define DOWN point{ 0, -1 }

struct color {
    int r;
    int g;
    int b;
};
struct tile_rule {
    color c;
    char sym;
};
struct mon_rule {
    color c;
    std::string name;
    int density;
};
struct item_rule {
    color c;
    std::string name;
    int chance;
    std::pair<int, int> repeat;
};

struct ruleBook {
    std::vector<tile_rule> tile_rules;
    std::vector<mon_rule> mon_rules;
    std::vector<item_rule> item_rules;
};

struct imageData {
    int width;
    int height;
    int bitdepth;
    int chardepth;
    int blocksize;
};

bool in_rect( point p, rect r ) {
    if( p.first >= r.first.first &&
        p.first <= r.second.first &&
        p.second >= r.first.second &&
        p.second <= r.second.second ) {
        return true;
    }
    return false;
}

point get_adjacent( point origin, point direction ) {
    point result = point( origin.first + direction.first, origin.second + direction.second );

    return result;
}

std::string tab( int n ) {
    std::string out = "";
    for( int i = 0; i < n; i++ ) {
        out += TAB;
    }
    return out;
}

int pti( point p, int row ) {
    int ret;
    return ret = p.first + ( row * p.second );
}

bool col_eq( color a, color b ) {
    if( a.r == b.r &&
        a.g == b.g &&
        a.b == b.b ) {
        return true;
    }
    return false;
}

ruleBook parse_rules() {
    // Modes
    // 1: [T] Tile
    // 2: [M] Monster
    // 3: [I] Item

    int mode = 0;

    std::vector<tile_rule> tile_rules;
    std::vector<mon_rule> mon_rules;
    std::vector<item_rule> item_rules;

    std::ifstream file( "rules.txt" );
    std::string line;
    if( file.is_open() ) {
        while( !file.eof() ) {
            line.clear();
            std::getline( file, line );

            if( line.compare( "[T]" ) == 0 ) {
                std::cout << "TILE MODE\n";
                mode = 1;
                continue;
            }
            if( line.compare( "[M]" ) == 0 ) {
                std::cout << "MON MODE\n";
                mode = 2;
                continue;
            }
            if( line.compare( "[I]" ) == 0 ) {
                std::cout << "ITEM MODE\n";
                mode = 3;
                continue;
            }

            //Get Color
            char* pEnd;
            int b = std::strtol( line.c_str(), &pEnd, 10 );
            int g = std::strtol( pEnd, &pEnd, 10 );
            int r = std::strtol( pEnd, &pEnd, 10 );

            //Tile
            if( mode == 1 ) {
                char sym = pEnd[1];
                tile_rules.push_back( tile_rule{ color{ r, g, b }, sym } );
            }
            
            //Monster
            if( mode == 2 ) {
                std::string name;
                std::string s_density = "";
                int density;

                bool startName = false;
                bool endName = false;
                
                bool done = false;
                for( int i = 0; i < line.size(); i++ ) {
                    if( !done ) {
                        if( line[i] == '=' ) {
                            startName = true;
                            continue;
                        }
                        if( startName && !endName ) {
                            if( line[i] != ' ' ) {
                                name += line[i];
                            } else {
                                endName = true;
                            }
                        }
                        if( endName == true && line[i] != ';' ) {
                            s_density += line[i];
                        }
                    }
                }
                if( s_density.compare( "" ) != 0 ) {
                    density = std::stoi( s_density, nullptr, 10 );

                    mon_rules.push_back( mon_rule{ color{ r, g, b }, name, density } );
                }
            }
            //Item
            if( mode == 3 ) {
                std::string name;
                std::string s_chance = "";

                std::string start = "";

                bool startName = false;
                bool endName = false;

                for( int i = 0; i < line.size(); i++ ) {
                    if( line[i] == '=' ) {
                        startName = true;
                        continue;
                    }
                    if( startName && !endName ) {
                        if( line[i] != ' ' ) {
                            name += line[i];
                        } else {
                            endName = true;
                            continue;
                        }
                    }
                    if( endName == true ) {
                        start = line.substr( i, line.size()-i-1 );
                        break;
                    }
                }

                char* pEnd;
                int chance = std::strtol( start.c_str(), &pEnd, 10 );
                int repeat_first = std::strtol( pEnd, &pEnd, 10 );
                int repeat_second = std::strtol( pEnd, nullptr, 10 );

                std::pair<int, int> repeat{ repeat_first, repeat_second };

                item_rules.push_back( item_rule{ color{ r, g, b }, name, chance, repeat } );
            }
        }
    }
    ruleBook rules{ tile_rules, mon_rules, item_rules };

    return rules;
}

char checkTileRules( std::vector<tile_rule> rules, color c ){

    for( int i = 0; i < rules.size(); i++ ) {

        if( col_eq( rules[i].c, c ) ) {
            return rules[i].sym;
        }
    }

    //default to space
    return ' ';
}

mon_rule checkMonRules( std::vector<mon_rule> rules, color c ) {

    for( int i = 0; i < rules.size(); i++ ) {

        if( col_eq( rules[i].c, c ) ) {
            return rules[i];
        }
    }
    //default to space
    return mon_rule{ color{ 255, 255, 255 }, "", 0 };
}

item_rule checkItemRules( std::vector<item_rule> rules, color c ) {

    for( int i = 0; i < rules.size(); i++ ) {

        if( col_eq( rules[i].c, c ) ) {
            return rules[i];
        }
    }
    //default to space
    return item_rule{ color{ 255, 255, 255 }, "", 0, std::pair<int, int>{ 0, 0 } };
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

imageData parseImage( std::vector<char> &data ) {
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
        sizebytes.push_back( data[i] );
    }
    int infosize = toInt( sizebytes );

    std::cout << "Infoheader size: " << infosize << '\n';

    std::vector<char> info;
    for( int i = 0; i < infosize; i++ ) {
        info.push_back( data[i] );
    }

    std::cout << "\n";
    translate( info );
    std::cout << "\n";

    const int WIDTH_OFFSET = 4;
    const int HEIGHT_OFFSET = 8;
    const int BITDEPTH_OFFSET = 14;
    std::cout << ". Getting Width...";
    std::vector<char> widthbytes;
    for( int i = 0; i < 4; i++ ) {
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

    std::cout << ".   Width: " << width << '\n';
    std::cout << ".   Height: " << height << '\n';
    std::cout << ".   Bit Depth: " << bitdepth << '\n';
    std::cout << ".   Char Depth: " << chardepth << '\n';
    std::cout << ".   Pixels: " << blocksize << '\n';

    for( int i = 0; i < infosize; i++ ) {
        data.erase( data.begin() );
    }
    std::cout << "Done\n";

    if( bitdepth != 24 ) {
        std::cout << "ERR: Bitmap must be 24-bit";
    }

    return imageData{ width, height, bitdepth, chardepth, blocksize };
}

std::vector<color> parseColors( std::vector<char> data, imageData image ) {
    std::vector<color> colors;

    for( int i = 0; i < image.blocksize; i++ ) {
        unsigned char r = data[i];
        unsigned char g = data[++i];
        unsigned char b = data[++i];

        colors.push_back( color{ (int)r, (int)g, (int)b } );
    }

    return colors;
}

std::vector<mon_spawn> findMonSpawns( imageData mapData, std::vector<color> colors, color test, ruleBook rules ) {
    std::vector<mon_spawn> spawns;
    
    for( int i = 0; i < mapData.height; i++ ) {
        for( int j = 0; j < mapData.width; j++ ) {
            point p = point{ j, i };
            int w = mapData.width;
            color col = colors[pti( p, w )];

            //Find monster area
            for( int r = 0; r < rules.mon_rules.size(); r++ ) {
                if( col_eq( col, rules.mon_rules[r].c ) ) {
                    bool done = false;
                    for( int k = 0; k < spawns.size(); k++ ) {
                        if( in_rect( point{ j,i }, spawns[k].first ) ) {
                            done = true;
                        }
                    }
                    if( !done ) {
                        for( int jj = j; jj < mapData.width && !done; jj++ ) {
                            point pp = point{ jj, i };
                            point next = get_adjacent( pp, RIGHT );
                            color nextCol = colors[pti( next, w )];

                            if( !col_eq( nextCol, test ) ) {
                                for( int ii = i; ii < mapData.height && !done; ii++ ) {
                                    point ppp = point{ jj, ii };
                                    next = get_adjacent( ppp, UP );
                                    nextCol = colors[pti( next, w )];

                                    if( !col_eq( nextCol, test ) ) {
                                        done = true;
                                        rect monBlock;

                                        monBlock.first = p;
                                        monBlock.second = ppp;

                                        spawns.push_back( mon_spawn{ monBlock, rules.mon_rules[r] } );
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return spawns;
}

std::vector<item_spawn> findItemSpawns( imageData mapData, std::vector<color> colors, color test, ruleBook rules ) {
    std::vector<item_spawn> spawns;

    for( int i = 0; i < mapData.height; i++ ) {
        for( int j = 0; j < mapData.width; j++ ) {
            point p = point{ j, i };
            int w = mapData.width;
            color col = colors[pti( p, w )];

            //Find Item area
            for( int r = 0; r < rules.item_rules.size(); r++ ) {
                if( col_eq( col, rules.item_rules[r].c ) ) {
                    bool done = false;
                    for( int k = 0; k < spawns.size(); k++ ) {
                        if( in_rect( point{ j,i }, spawns[k].first ) ) {
                            done = true;
                        }
                    }
                    if( !done ) {
                        for( int jj = j; jj < mapData.width && !done; jj++ ) {
                            point pp = point{ jj, i };
                            point next = get_adjacent( pp, RIGHT );
                            color nextCol = colors[pti( next, w )];

                            if( !col_eq( nextCol, test ) ) {
                                for( int ii = i; ii < mapData.height && !done; ii++ ) {
                                    point ppp = point{ jj, ii };
                                    next = get_adjacent( ppp, UP );
                                    nextCol = colors[pti( next, w )];

                                    if( !col_eq( nextCol, test ) ) {
                                        done = true;
                                        rect monBlock;

                                        monBlock.first = p;
                                        monBlock.second = ppp;

                                        spawns.push_back( item_spawn{ monBlock, rules.item_rules[r] } );
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return spawns;
}

int main()
{
    /**************
    * User Inputs *
    **************/

    std::string fill_ter;
    std::string palette;

    ruleBook rules = parse_rules();
    std::cout << "Tile Rules: " << rules.tile_rules.size() << '\n';
    std::cout << "Monster Rules: " << rules.mon_rules.size() << '\n';
    std::cout << "Item Rules: " << rules.item_rules.size() << '\n';

    /*
    for( int i = 0; i < rules.tile_rules.size(); i++ ) {
        std::cout << rules.tile_rules[i].c.r << " " << rules.tile_rules[i].c.g << " " << rules.tile_rules[i].c.b << " " << rules.tile_rules[i].sym << "\n";
    }
    for( int i = 0; i < rules.mon_rules.size(); i++ ) {
        std::cout << rules.mon_rules[i].c.r << " " << rules.mon_rules[i].c.g << " " << rules.mon_rules[i].c.b << " ";
        std::cout << rules.mon_rules[i].name << " " << rules.mon_rules[i].density << "\n";
    }
    for( int i = 0; i < rules.item_rules.size(); i++ ) {
        std::cout << rules.item_rules[i].c.r << " " << rules.item_rules[i].c.g << " " << rules.item_rules[i].c.b << " ";
        std::cout << rules.item_rules[i].name << " " << rules.item_rules[i].chance << " " << rules.item_rules[i].repeat.first << " " << rules.item_rules[i].repeat.second << "\n";
    }
    */

    std::string filename;

    std::cout << "Filename: ";
    std::getline( std::cin, filename );

    std::cout << "Fill Terrain (Leave empty for \"t_dirt\"): ";
    std::getline( std::cin, fill_ter );
    if( fill_ter.compare( "" ) == 0 ) {
        fill_ter = "t_dirt";
    }

    std::cout << "Palette ID: ";
    std::getline( std::cin, palette );

    /*********
    * Do Map *
    *********/

    std::cout << "Loading Map Image...";
    std::vector<char> data = readImage( (filename + ".bmp").c_str() );
    std::cout << "Done\n";

    std::cout << ". Parsing Image...";
    imageData mapData = parseImage( data );

    std::cout << ". Parsing Colors...";
    std::vector<color> colors = parseColors( data, mapData );
    std::cout << "Done\n";

    std::cout << ". Building Tiles...";
    std::vector<char> out;
    for( int i = 0; i < colors.size(); i++ ) {
        out.push_back( checkTileRules( rules.tile_rules, colors[i] ) );
    }
    std::cout << "Done\n";
    std::cout << "Tiles Done\n";

    /**************
    * Do Monsters *
    **************/

    // { "monster": "GROUP_MALL", "x": [ 2, 23 ], "y": [ 2, 23 ], "density": 0.15 },

    std::cout << "Loading Monster Overlay...";
    std::vector<char> m_data = readImage( ( filename + "_m.bmp" ).c_str() );
    std::cout << "Done\n";

    std::cout << ". Parsing Image...";
    imageData m_mapData = parseImage( m_data );
    std::cout << "Done\n";

    std::cout << ". Parsing Colors...";
    std::vector<color> m_colors = parseColors( m_data, m_mapData );
    std::cout << "Done\n";

    std::cout << ". Finding Spawns...";
    std::vector<mon_spawn> m_spawns = findMonSpawns( m_mapData, m_colors, color{ 0,0,255 }, rules );
    std::cout << "Done\n";
    std::cout << "Monsters Done\n";

    /**************
    * Do Items *
    **************/

    // { "group": "mechanics", "x" : [61, 64] , "y" : 13, "chance" : 90, "repeat" : [1, 3] },

    std::cout << "Loading Item Overlay...";
    std::vector<char> i_data = readImage( ( filename + "_i.bmp" ).c_str() );
    std::cout << "Done\n";

    std::cout << ". Parsing Image...";
    imageData i_mapData = parseImage( i_data );
    std::cout << "Done\n";

    std::cout << ". Parsing Colors...";
    std::vector<color> i_colors = parseColors( i_data, i_mapData );
    std::cout << "Done\n";

    std::cout << ". Finding Spawns...";
    std::vector<item_spawn> i_spawns = findItemSpawns( i_mapData, i_colors, color{ 255,0,0 }, rules );
    std::cout << "Done\n";
    std::cout << "Items Done\n";

    /*************
    * Write JSON *
    *************/

    //Create Map
    std::ofstream outfile;
    outfile.open( (filename + ".json").c_str() );

    outfile << "[\n";
    outfile << tab(1) << "{\n";

    outfile << tab(2) << "\"type\": \"mapgen\",\n";
    outfile << tab(2) << "\"method\": \"json\",\n";

    int mapwidth = mapData.width / 24;
    int mapheight = mapData.height / 24;

    outfile << tab(2) << "\"om_terrain\": ";
    if( mapwidth == 1 && mapheight == 1 ) {
        outfile << "\"" << filename << "\",\n";
    } else {
        outfile << " [\n";
        for( int i = 0; i < mapheight; i++ ) {
            outfile << tab( 3 ) << "[ ";
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
        outfile << tab( 3 ) << "],\n";
    }

    outfile << tab(2) << "\"object\": {\n";
    outfile << tab(3) << "\"fill_ter\": \"" << fill_ter << "\"\n";

    outfile << tab( 3 ) << "\"rows\": [\n";
    for( int i = 0; i < mapData.height; i++ ) {
        outfile << tab(4) << "\"";
        for( int j = 0; j < mapData.width; j++ ) {
            outfile << out[j + ( mapData.width * i )];
        }
        outfile << "\"";
        if( i+1 < mapData.height ) {
            outfile << ",";
        }
        outfile << '\n';
    }
    outfile << tab( 3 ) << "],\n";

    outfile << tab( 3 ) << "\"terrain\": { }, \n";
    outfile << tab( 3 ) << "\"furniture\": { }, \n";
    if( palette.compare( "" ) != 0 ) {
        outfile << tab( 3 ) << "\"palettes\": { " << "\"" << palette << "\" }, \n";
    } else {
        outfile << tab( 3 ) << "\"palettes\": { }, \n";
    }
    
    outfile << tab( 3 ) << "\"place_items\": [ \n";
    int chance = 80;
    std::pair<int, int> repeat = { 1, 3 };

    // { "group": "mechanics", "x" : [61, 64] , "y" : 13, "chance" : 90, "repeat" : [1, 3] },
    for( int i = 0; i < i_spawns.size(); i++ ) {
        outfile << tab( 4 ) << "{ \"group\": \"" << i_spawns[i].second.name << "\", \"x\": [ ";
        outfile << i_spawns[i].first.first.first << ", " << i_spawns[i].first.first.second << " ], \"y\": [ ";
        outfile << i_spawns[i].first.second.first << ", " << i_spawns[i].first.second.second << " ], ";
        outfile << "\"chance\": " << i_spawns[i].second.chance; 
        if( i_spawns[i].second.repeat.first != 0 && i_spawns[i].second.repeat.second != 0 ) {
            outfile << " \"repeat\": [ " << i_spawns[i].second.repeat.first << ", " << i_spawns[i].second.repeat.second << " ]";
        }
        outfile << " }";
        if( ( i + 1 ) >= i_spawns.size() ) {
            outfile << "\n";
        } else {
            outfile << ",\n";
        }
    }

    outfile << tab( 3 ) << "\"place_monsters\": [ \n";

    int density = 0.15;
    // { "monster": "GROUP_MALL", "x": [ 2, 23 ], "y": [ 2, 23 ], "density": 0.15 },
    for( int i = 0; i < m_spawns.size(); i++ ) {
        outfile << tab(4) << "{ \"monster\": \"" << m_spawns[i].second.name << "\", \"x\": [ ";
        outfile << m_spawns[i].first.first.first << ", " << m_spawns[i].first.first.second << " ], \"y\": [ ";
        outfile << m_spawns[i].first.second.first << ", " << m_spawns[i].first.second.second << " ], ";
        outfile << "\"density\": " << m_spawns[i].second.density << " }";
        if( ( i + 1 ) >= m_spawns.size() ) {
            outfile << "\n";
        } else {
            outfile << ",\n";
        }
    }

    outfile << tab( 2 ) << "}\n";
    outfile << tab( 1 ) << "}\n";
    outfile << "]\n";

    outfile.close();

    //Create Extras
    outfile.open( ( filename + "_extras.json" ).c_str() );

    
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
    
    std::cout << "Map Saved to: " << filename << ".json\n";
    std::cout << "Overmap Saved to: " << filename << "_extras.json\n";


    std::cout << '\n';
    
}