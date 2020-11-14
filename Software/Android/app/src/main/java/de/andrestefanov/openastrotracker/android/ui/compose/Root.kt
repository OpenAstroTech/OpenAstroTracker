package de.andrestefanov.openastrotracker.android.ui.compose

import androidx.compose.runtime.Composable
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.navigate
import androidx.navigation.compose.rememberNavController
import de.andrestefanov.openastrotracker.android.TelescopeStateView
import de.andrestefanov.openastrotracker.android.ui.compose.telescopes.TelescopeSelection


@Composable
fun Root() {

    val navController = rememberNavController()

    NavHost(navController = navController, startDestination = "telescopes") {
        composable("telescopes") {
            TelescopeSelection { selectedTelescope ->
                with(selectedTelescope) {
                    navController.navigate("telescope/${connection.protocol}/${connection.address}/state")
                }
            }
        }
        composable("telescope/{protocol}/{address}/state") { backStackEntry ->
            TelescopeStateView(
                protocol = backStackEntry.arguments!!.getString("protocol")!!,
                address = backStackEntry.arguments!!.getString("address")!!
            )
        }
    }
}