package de.andrestefanov.openastrotracker.android.repository

import de.andrestefanov.openastrotracker.android.meade.network.UDPSocketFactory
import de.andrestefanov.openastrotracker.meade.MeadeTelescope
import de.andrestefanov.openastrotracker.meade.network.skyfi.SkyFiDiscovery
import io.ktor.util.network.*
import kotlinx.coroutines.flow.distinctUntilChanged
import kotlinx.coroutines.flow.scan
import java.net.NetworkInterface

open class TelescopeRepository {

    // TODO: listen to interfaces in a reactive way (if e.g. wifi state or ip changes)
    private val broadcastAddress = NetworkInterface.getNetworkInterfaces().toList()
        .filterNot { it.isLoopback }
        .filter { it.isUp }
        .map { it.interfaceAddresses }
        .flatten()
        .filterNot { it.broadcast == null }
        .filterNot { it.address == null }
        .map {
            NetworkAddress(it.broadcast.hostAddress, 4031)
        }
        .first()

    open fun discoverScopes() =
        SkyFiDiscovery(UDPSocketFactory()).discover(broadcastAddress)
            .scan(emptySet<MeadeTelescope>(), operation = { accumulator, value ->
                accumulator.toMutableSet().apply {
                    add(value)
                }
            }).distinctUntilChanged()

}