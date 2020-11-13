package de.andrestefanov.openastrotracker.meade.network.skyfi

import de.andrestefanov.openastrotracker.meade.MeadeTelescope
import de.andrestefanov.openastrotracker.meade.network.TCPConnection
import de.andrestefanov.openastrotracker.meade.network.UDPSocket
import de.andrestefanov.openastrotracker.meade.utils.loge
import de.andrestefanov.openastrotracker.meade.utils.logv
import io.ktor.util.network.*
import io.ktor.utils.io.core.*
import io.ktor.utils.io.errors.*
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.channels.awaitClose
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.callbackFlow
import kotlinx.coroutines.flow.flowOn
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch

@Suppress("EXPERIMENTAL_API_USAGE")
class SkyFiDiscovery(private val udpSocketFactory: UDPSocket.Factory) {

    companion object {

        private const val TAG = "SkyFiDiscovery"

        private const val COMMUNICATION_PORT = 4030

        private const val BROADCAST_DELAY = 1000L
    }

    fun discover(
        remoteAddress: NetworkAddress
    ) = callbackFlow {
        val socket = udpSocketFactory.create(remoteAddress)

        // send discovery packets (broadcast)
        launch {
            do {
                logv(TAG, "Sending discovery datagram")

                val message = "skyfi:?"
                socket.send(message.toByteArray())

                delay(BROADCAST_DELAY)
            } while (isActive and (BROADCAST_DELAY > 0))
        }

        var closed = false

        // receive incoming packets
        launch {
            while (isActive) {
                logv(TAG, "Waiting for incoming datagram")

                val buffer = ByteArray(1024)

                val size: Int = try {
                    socket.receive(buffer)
                } catch (e: IOException) {
                    if (!closed) throw e else continue
                }
                val message = String(buffer, 0, size)

                logv(TAG, "Received message $message")

                // check if the datagram matches expected response regex
                // this way we ignore our own broadcast and not parsable datagrams
                // expected result of form: skyfi:OATerScope@192.168.178.76
                val regex = Regex("""skyfi:(.*)@(.*)""")
                if (!regex.matches(message)) {
                    logv(TAG, "message does not match regex, skip")
                    continue
                }

                logv(TAG, "A device answered with: $message")

                // parse the message by using regex
                regex.find(message)?.let { result ->
                    val (name, host) = result.destructured
                    val telescope = MeadeTelescope(
                        name,
                        TCPConnection(host, COMMUNICATION_PORT)
                    )

                    logv(TAG, "Discovered $telescope")

                    offer(telescope)
                } ?: run {
                    loge(TAG, "Failed to parse message=\"$message\"")
                }

            }
        }

        awaitClose {
            logv(TAG, "Closing")
            closed = true
            socket.close()
        }
    }.flowOn(Dispatchers.IO)

}