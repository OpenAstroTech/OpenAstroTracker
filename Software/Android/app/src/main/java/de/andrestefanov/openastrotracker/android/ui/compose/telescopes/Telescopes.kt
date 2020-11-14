package de.andrestefanov.openastrotracker.android.ui.compose.telescopes

import android.util.Log
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumnFor
import androidx.compose.material.Button
import androidx.compose.material.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.livedata.observeAsState
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.viewModel
import androidx.ui.tooling.preview.Preview
import de.andrestefanov.openastrotracker.android.ui.OpenAstroTrackerTheme
import de.andrestefanov.openastrotracker.android.utils.MockTCPConnection
import de.andrestefanov.openastrotracker.android.viewmodel.TelescopeListViewModel
import de.andrestefanov.openastrotracker.meade.MeadeTelescope

private const val TAG = "TelescopeSelection"

@Composable
fun TelescopeSelection(
    modifier: Modifier = Modifier,
    telescopeListViewModel: TelescopeListViewModel = viewModel(),
    onTelescopeSelected: (MeadeTelescope) -> Unit
) {

    val scopes: Set<MeadeTelescope> by telescopeListViewModel.scopes.observeAsState(emptySet())

    Column(modifier = modifier) {
        Text(
            modifier = modifier.fillMaxWidth(),
            text = "Telescopes"
        )
        TelescopeList(items = scopes.toList()) { selectedTelescope ->
            Log.d(TAG, "TelescopeSelection: selected telescope $selectedTelescope")
            onTelescopeSelected(selectedTelescope)
        }
    }
}

@Composable
private fun TelescopeList(
    items: List<MeadeTelescope>,
    onScopeSelected: (MeadeTelescope) -> Unit
) = LazyColumnFor(
    items = items,
    contentPadding = PaddingValues(top = 16.dp, start = 16.dp, end = 16.dp, bottom = 0.dp)
) { scope ->
    Button(
        onClick = { onScopeSelected(scope) },
        modifier = Modifier.fillParentMaxWidth().padding(bottom = 16.dp)
    ) {
        Text(text = scope.name, modifier = Modifier.padding(8.dp), color = Color.White)
    }
}

@Preview(
    "Telescope selection (day)",
    showBackground = true
)
@Composable
private fun TelescopeSelectionPreviewDay() {
    OpenAstroTrackerTheme {
//        TelescopeList(
//            items = listOf(
//                MeadeTelescope("TestScope", MockTCPConnection()),
//                MeadeTelescope("OpenAstroTracker", MockTCPConnection())
//            )
//        ) {
//
//        }
        TelescopeSelection {

        }
    }
}

@Preview("Telescope selection (night)", showBackground = true, backgroundColor = 0x000000)
@Composable
private fun TelescopeSelectionPreviewNight() {
    OpenAstroTrackerTheme(darkTheme = true) {
        TelescopeList(
            items = listOf(
                MeadeTelescope("TestScope", MockTCPConnection()),
                MeadeTelescope("OpenAstroTracker", MockTCPConnection())
            )
        ) {

        }
//        TelescopeSelection {
//
//        }
    }
}