

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#define CURVE_TYPE_M 0
#define CURVE_TYPE_S 1
#define CURVE_TYPE_C 2
#define COORDINATE_X1 0 //Outbound coordinate
#define COORDINATE_Y1 1
#define COORDINATE_X2 2 //Inbound Coordinate
#define COORDINATE_Y2 3
#define COORDINATE_X3 4 //Destination Coordinate
#define COORDINATE_Y3 5

typedef struct Coords{
    double x;
    double y;
} Coords;


typedef struct ProtoPath{
    std::vector<Coords> in;
    std::vector<Coords> out;
    std::vector<Coords> points;
    int colour[3];
} ProtoPath;

std::string pathToString(ProtoPath &path){
    std::string output = "";
    for(int i =0; i<path.points.size();i++)
    {
        output += "outbound: ";
        output += std::to_string(path.out[i].x);
        output +=',';
        output += std::to_string(path.out[i].y);
        output +='\n';
        output += "inbound: ";
        output += std::to_string(path.in[i].x);
        output +=',';
        output += std::to_string(path.in[i].y);
        output +='\n';
        output += "dest: ";
        output += std::to_string(path.points[i].x);
        output +=',';
        output += std::to_string(path.points[i].y);
        output +='\n';
    }
    return output;
}

int main(){
    std::ifstream vectorFile;
    vectorFile.open("topography.svg");
    std::string path = "";
    std::string buffer;
    std::vector<std::string> paths;
    bool readingPath = false;
    // Split paths into string.
    while(getline(vectorFile,buffer)){
        if(readingPath)
        {
            path+= buffer;
            if(buffer.find(">") != -1){
                readingPath = false;
                paths.push_back(path);
                path = "";
            }
        } else {
            if(buffer.find("path") != -1){
                // then this line is useful.
                readingPath = true;
                path = buffer;
            }
        }
    }
    // std::string rawInput = "<path fill=\"none\" stroke=\"#5B8B3B\" stroke-miterlimit=\"10\" d=\"M438.415,361.343c0,0,12.667-45,49.667-37s89.666,15.667,63.333,69.667s-34.667,64.611-25,105.806s45.667,113.193-8,136.527c-26.833,11.667-62.167,22.25-93.334,22.042s-58.167-11.208-68.333-42.708c-20.333-63-32-118.774-9.333-141.554C370.082,451.343,423.415,405.009,438.415,361.343z\"/>";
    std::size_t index;
    std::string colour;
    std::string data;
    std::string rawInput = paths[0];
    // get the colour substring.
    index = rawInput.find('#');
    colour = rawInput.substr(index+1,6);
    std::cout <<"Colour:"<< colour << std::endl;// protoPath.colour = colour;

    // Get the coordinate list
    index = rawInput.find("d=");
    data = rawInput.substr(index+4); // skip `d= "M`
    // remove last 3 elements.
    data.pop_back();
    data.pop_back();
    data.pop_back();
    data.pop_back();
    // Read through all coordinates.
    // Read starting position x and y.
    
    // First, replace - with ,-
    std::cout << "#########" << std::endl;
    std::cout << data << std::endl;
    for(int n=0; n<data.length(); n++)
    {
        if(data[n] == '\t' || data[n] == ' '){
            data.erase(n,1);
            n--;
        }
    }
    for(int n=0; n<data.length(); n++)
    {
        // cout << n << endl;
        // Replace '-' with ',-'
        if(data[n] == '-' && n != 0)
        {
            if(data[n-1] >= '0' && data[n-1] <= '9'){
                data.insert(n,",");
                n++;
            }
        }
    }
    std::cout << "###########" << std::endl;
    std::cout << data << std::endl;
    for(int n=0; n<data.length(); n++)
    {
        // Replace 'M' with ',M'
        if((data[n] == 'm' || data[n] == 'M' || data[n] == 'c' || data[n] == 'C' || data[n] == 's' || data[n] == 'S') && n != 0)
        {
            data.insert(n,",");
            n++;
        }
        // if(data[n] == 's' || data[n] == 'S'){
        //     // data.erase(n+1,1);
        //     data.insert(n,",");
        //     n++;
        // }
    }
    std::cout << "###########3" << std::endl;
    std::cout << data << std::endl;
    // Collect x and y for first "move command"
    // Check for 'c' command
    // If using C x,y,c1x,c1y,c2x,c2y
    std::string token = "";
    ProtoPath protopath;
    Coords outbound;
    Coords inbound;
    Coords dest;
    int readingCoor = 4;
    int curveType = CURVE_TYPE_M;
    double coordinateSet[6];
    double converted;
    for(int k=0;k<6;k++){
        coordinateSet[k]=0;
    }
    char c;
    while(data.length()>0){
        c = data[0];
        data.erase(0,1);
        if(c=='s' || c== 'S' || c=='c' || c=='C' || c=='M' || c=='m')
        {
            switch (c)
            {
                case 's':
                    // do the case below
                case 'S':
                    // We need to do something for S curves
                    readingCoor = 2; // We don't need to read the outbound vector for this set.
                    // Reflect our former inbound vector.
                    // Set previous dest as origin. (Reflect relative to that point)
                    coordinateSet[0] = coordinateSet[2] - coordinateSet[4];
                    coordinateSet[1] = coordinateSet[3] - coordinateSet[5];
                    coordinateSet[0] *= -1;
                    coordinateSet[1] *= -1;
                    coordinateSet[0] += coordinateSet[4];
                    coordinateSet[1] += coordinateSet[5];
                    break;
                case 'c':
                    // Do the case below:
                case 'C':
                    // We need to do something for cubic curves. 
                    readingCoor = 0; //Use all 6 numbers of coordinate set.
                    break;
                case 'm':
                    // do the case below
                case 'M':
                    // Inbound and outbound vectors are 0,0
                    for(int k =0; k<4;k++){
                        coordinateSet[k] = 0;
                    }
                    readingCoor = 4; //we only need to read 1 set of coordinates.
                    break;
                default:
                    break;
            }
        }
        else if(c == ',')
        {
            // We've finished processing a coordinate.
            // Convert token to double.
            std::cout << "CONVERTED: " << token << "##" << std::endl;
            converted = std::stod(token);
            token = "";
            // Load double into the correct location.
            coordinateSet[readingCoor] = converted;
            // Figure out if we've
            // based on the coordinate we're up to, load it into the correct one.  
            readingCoor++;
            if (readingCoor > 5){
                // if we've read the last number (destination y), load our numbers into a datastructure and 
                // Load coordinates into Path.
                outbound.x = coordinateSet[0];
                outbound.y = coordinateSet[1];
                protopath.out.push_back(outbound);
                inbound.x = coordinateSet[2];
                inbound.y = coordinateSet[3];
                protopath.in.push_back(inbound);
                dest.x = coordinateSet[4];
                dest.y = coordinateSet[5];
                protopath.points.push_back(dest);
                
                //Reset readingCoor to appropriate value:
                switch (curveType)
                {
                    case CURVE_TYPE_M:
                        readingCoor = 4; // 'skip' the first 4 numbmers (inbound and outbound vectors)
                        break;
                    case CURVE_TYPE_C:
                        readingCoor = 0;
                        break;
                    case CURVE_TYPE_S:
                        readingCoor = 2; // 'skip' the first 2 numbers.
                    default:
                        break;
                }
            }
            
        }
        else //I guess it must be a number?
        {
            token += c;
        }
        
    //     // depending on the current character do different things.
    //     // If it's not a number, then it's a command?
    //     // If it's a comma
    //     // We need to load this number into the correct array.
    //         // Could be x or y
    //         // Could be using curve or not.
    //     // If it's '-'
    }
    std::cout << pathToString(protopath) << std::endl;

    std::string hex = "5B8B3B";
    for(int b =0;b<6;b+=2)
    {
        int h = std::stoi(hex.substr(b,2),0,16);
        std::cout << h << std::endl;

    }
    std::vector<int> seven;
    seven.push_back(14);
    seven[0] /= 2;
    std::cout << seven[0] << std::endl;
    // Now... Get 3 digits from it.
    // First 3 digits.
}