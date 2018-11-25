#include <Arduino.h>
#include "UdpHandler.h"
#include "ESPAsyncudp.h"

uint8_t prevDmx[512];

UdpHandler::UdpHandler(Stream *stream)
{    
    if(!stream) serial = &Serial; else serial = stream;
    udp = new AsyncUDP();
}

bool UdpHandler::start(int port, DmxCallbackFunction fn)
{
    handler = fn;
    serial->print("UDP connecting...");
    if(udp->connected()) udp->close();
    if(udp->listen(port)) {        
        memset(prevDmx, 0, 512 * sizeof(uint8_t));
        serial->print("UDP is listening on port ");
        serial->println(port);        
        udp->onPacket([&](AsyncUDPPacket packet) { 
            this->handle(packet, prevDmx); 
        });
        return true;
    } else {
        serial->print("UDP error");
    }
    return false;           
}

void UdpHandler::handle(AsyncUDPPacket packet, uint8_t prevDmx[])
{
    ArtnetDmxFull *artnet = (ArtnetDmxFull *)packet.data();
            
    if(packet.length() <= sizeof(ArtnetDmxFull)) 
    {
        if(artnet->OpCode == 0x5000 && artnet->length >= 2)
        {
            // sume controllers can send wrong number of channels so limit it to what we've got
            artnet->length = min((int)ntohs(artnet->length), (int)packet.length() - 18);
/*
            serial->print(" DMX: ");
            serial->print(packet.length());
            serial->print(" length: ");
            serial->print(artnet->length);
*/
            if(isFirst || memcmp(prevDmx, artnet->data, artnet->length) != 0)
            {
                for(int i=0; i<artnet->length; i++)
                {
                    if(prevDmx[i] != artnet->data[i])
                    {
                        prevDmx[i] = artnet->data[i];
                        handler(i, artnet->data[i]);
                    }
                }
                isFirst = false;
            }
        }
        else
        {
            serial->print("From: ");
            serial->print(packet.remoteIP());
            serial->print(" Data: ");
            for(int i=0; i<packet.length(); i++)
            {
                serial->print(packet.data()[i], HEX);
                serial->print(" ");
            }
            serial->println();
        }
    }    
}