#ifndef PACKETIZER_H
#define PACKETIZER_H

/* 
 * This header defines the outgoing connection stream
 */

class Packetizer : public TunnelStream, 
                   public boost::enable_shared_from_this<Packetizer> {

};

#endif
