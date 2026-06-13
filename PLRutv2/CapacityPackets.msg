import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

class CapacityPacket extends inet::FieldsChunk
{
    unsigned int IDtalk;
    string carId;
    string currentEdge;
    string destinationEdge;
    string routeEdges;
    bool arrived;
    bool hasNewRoute;
    bool wait;
    string newRouteEdges;
}