package de.andrestefanov.openastrotracker.android

import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumnFor
import androidx.compose.foundation.text.BasicText
import androidx.compose.material.Button
import androidx.compose.material.MaterialTheme
import androidx.compose.material.Surface
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.livedata.observeAsState
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.setContent
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.viewModel
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.navigate
import androidx.navigation.compose.rememberNavController
import androidx.ui.tooling.preview.Preview
import de.andrestefanov.openastrotracker.android.ui.OpenAstroTrackerTheme
import de.andrestefanov.openastrotracker.android.utils.MockTCPConnection
import de.andrestefanov.openastrotracker.android.viewmodel.TelescopeListViewModel
import de.andrestefanov.openastrotracker.android.viewmodel.TelescopeState
import de.andrestefanov.openastrotracker.android.viewmodel.TelescopeViewModel
import de.andrestefanov.openastrotracker.meade.MeadeTelescope
import de.andrestefanov.openastrotracker.meade.network.Connection

private const val TAG = "MainActivity"

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            OpenAstroTrackerTheme {
                // A surface container using the 'background' color from the theme
                Surface(color = MaterialTheme.colors.background) {
                    RootView()
                }
            }
        }
    }
}

@Composable
fun RootView() {

    val navController = rememberNavController()

    NavHost(navController = navController, startDestination = "telescopes") {
        composable("telescopes") {
            TelescopeSelection(
                onTelescopeSelected = {
                    with(it) {
                        navController.navigate("telescope/${connection.protocol}/${connection.address}/state")
                    }
                }
            )
        }
        composable("telescope/{protocol}/{address}/state") { backStackEntry ->
            TelescopeStateView(
                protocol = backStackEntry.arguments!!.getString("protocol")!!,
                address = backStackEntry.arguments!!.getString("address")!!
            )
        }
    }
}

@Composable
fun TelescopeSelection(
    telescopeListViewModel: TelescopeListViewModel = viewModel(),
    onTelescopeSelected: (MeadeTelescope) -> Unit
) {

    val scopes: Set<MeadeTelescope> by telescopeListViewModel.scopes.observeAsState(emptySet())

    TelescopeList(scopes = scopes.toList()) { selectedTelescope ->
        Log.d(TAG, "TelescopeSelection: selected telescope $selectedTelescope")
        onTelescopeSelected(selectedTelescope)
    }

}

@Composable
fun TelescopeStateView(
    protocol: String,
    address: String,
    telescopeViewModel: TelescopeViewModel = viewModel()
) {

    val state: TelescopeState? by telescopeViewModel.data(
        MeadeTelescope(
            "TODO",
            Connection.from(protocol, address)
        )
    ).observeAsState()

    BasicText(text = state.toString())
}

@Composable
fun TelescopeList(
    scopes: List<MeadeTelescope>,
    onScopeSelected: (MeadeTelescope) -> Unit
) = LazyColumnFor(
    items = scopes,
    contentPadding = PaddingValues(top = 16.dp, start = 16.dp, end = 16.dp, bottom = 0.dp)
) { scope ->
    Button(
        onClick = { onScopeSelected(scope) },
        modifier = Modifier.fillParentMaxWidth().padding(bottom = 16.dp)
    ) {
        BasicText(text = scope.name, modifier = Modifier.padding(8.dp))
    }
}

@Preview("Telescope list")
@Composable
fun DefaultPreview() {
    OpenAstroTrackerTheme {
        TelescopeList(
            scopes = listOf(
                MeadeTelescope("TestScope", MockTCPConnection()),
                MeadeTelescope("OpenAstroTracker", MockTCPConnection())
            )
        ) {

        }
//        TelescopeSelection()
    }
}