package de.andrestefanov.openastrotracker.android.communication.wifi

import android.util.Log
import kotlinx.coroutines.flow.asFlow
import kotlinx.coroutines.flow.flatMapMerge
import kotlinx.coroutines.flow.flow
import java.net.*

class SkyFiDiscovery {

    data class SkyFiScope(val name: String, val address: InetAddress)

    companion object {
        private const val TAG = "SkyFiDiscovery"
    }

    @Suppress("EXPERIMENTAL_API_USAGE")
    fun discover(name: String, port: Int) = listAllAddresses()
        .flatMapMerge { discover(it, port, name) }

    private fun listAllAddresses() = NetworkInterface.getNetworkInterfaces().toList()
            .filterNot { it.isLoopback }
            .filter { it.isUp }
            .map { it.interfaceAddresses }
            .flatten()
            .filterNot { it.broadcast == null }
            .filterNot { it.address == null }
            .asFlow()

    @Suppress("BlockingMethodInNonBlockingContext")
    private fun discover(interfaceAddress: InterfaceAddress, port: Int, name: String) = flow {
        Log.d(TAG, "discover: start discover for $name on address=$interfaceAddress, port=$port")
        val socket = DatagramSocket(port).apply {
            broadcast = true
            soTimeout = 1000
        }

        val buffer = "skyfi:$name?".toByteArray(Charsets.US_ASCII)
        val discoveryPacket = DatagramPacket(buffer, buffer.size, interfaceAddress.broadcast, port)
        socket.send(discoveryPacket)

        val buf = ByteArray(256)
        val packet = DatagramPacket(buf, buf.size)
        do {
            socket.receive(packet)
        } while (packet.address == interfaceAddress.address)

        emit(SkyFiScope(name, packet.address))

        socket.close()
    }

}