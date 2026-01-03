# Carta di Approccio Completa - Test ed Esempio

Questo esempio dimostra come generare una carta di approccio completa con tutte le feature avanzate di IOC_StarMap.

## Caratteristiche Incluse

### 🌟 Stelle
- Query da catalogo Gaia DR3
- Magnitudine limite 10.0
- Dimensioni proporzionali alla magnitudine
- Colori spettrali basati su temperatura

### 🔢 Identificatori
- **Numeri SAO**: Mostrati per stelle fino a mag 8.0
- **Nomi propri**: Sirio, Betelgeuse, Rigel, etc.
- **Designazioni Bayer**: α, β, γ Orionis
- **Designazioni Flamsteed**: 19 Ori, 24 Ori, etc.

### 🌌 Costellazioni
- **Linee delle costellazioni**: Collegamenti tra stelle principali
- **Confini delle costellazioni**: Limiti ufficiali IAU
- **Nomi delle costellazioni**: Etichette latine

### 📐 Coordinate
- Griglia RA/Dec ogni 5°
- Etichette coordinate sui bordi
- Orientamento: Nord in alto, Est a sinistra

### 🎨 Elementi Grafici
- Titolo e sottotitolo
- Bordo decorativo
- Bussola direzionale
- Scala angolare
- Legenda (opzionale)

## Build

```bash
cd build
cmake .. -DBUILD_EXAMPLES=ON
make approach_full_test
```

## Esecuzione

### Opzione 1: Eseguibile Standalone

```bash
cd build/examples
./approach_full_test
```

Output: `approccio_orione_completo.png` (1920x1920 px)

### Opzione 2: Con File JSON

```bash
cd build/examples
./generate_chart ../config_examples/approach_full_example.json
```

## Output Atteso

```
=== Test Carta di Approccio Completa ===

1. Inizializzazione cataloghi...
   ✓ Database SAO locale disponibile

2. Query stelle nella regione...
   Trovate 2847 stelle in 45 ms
   Stelle con SAO: 876/2847 (30.8%)

3. Generazione carta...
   Carta generata in 1234 ms

✓ Carta salvata: approccio_orione_completo.png
  Dimensioni: 1920x1920 px
  Memoria: 1456 KB

4. Statistiche della carta:
   --------------------------------
   Magnitudine: -0.42 - 10.00 (media: 7.32)

   Distribuzione magnitudini:
   mag 0: * (1)
   mag 1: ** (3)
   mag 2: *** (5)
   mag 3: ******* (12)
   mag 4: ************ (23)
   mag 5: ******************* (38)
   mag 6: ******************************** (67)
   mag 7: *************************************************** (124)
   mag 8: ********************************************************************** (198)
   mag 9: ******************************************************************************** (347)

   Stelle principali con SAO:
   Rigel           | SAO 131907 | mag -0.12 | 78.634°  -8.202°
   Betelgeuse      | SAO 113271 | mag  0.50 | 88.793°   7.407°
   Bellatrix       | SAO 112740 | mag  1.64 | 81.283°   6.350°
   Alnilam         | SAO 132346 | mag  1.69 | 84.053°  -1.202°
   Alnitak         | SAO 132444 | mag  1.77 | 85.190°  -1.943°
   Mintaka         | SAO 132263 | mag  2.23 | 83.002°  -0.299°
   Saiph           | SAO 132542 | mag  2.06 | 86.939°  -9.670°
   ...

=== Test completato con successo! ===
```

## Personalizzazione

### Modifica Regione

Cambia le coordinate del centro:

```cpp
config.center = core::EquatorialCoordinates(RA, Dec);
```

Esempi:
- **Orione**: `(85.0, 0.0)` - RA: 5h40m, Dec: 0°
- **M31 Andromeda**: `(10.68, 41.27)` - RA: 0h43m, Dec: +41°16'
- **Leone**: `(170.0, 12.0)` - RA: 11h20m, Dec: +12°
- **Scorpione**: `(247.35, -26.43)` - RA: 16h29m, Dec: -26°26'

### Modifica Campo Visivo

```cpp
config.fieldOfViewWidth = 30.0;   // 30° larghezza
config.fieldOfViewHeight = 30.0;  // 30° altezza
```

Standard:
- **Finder**: 30-40° (localizzazione generale)
- **Approach**: 15-25° (avvicinamento)
- **Detail**: 3-10° (osservazione)

### Modifica Magnitudine Limite

```cpp
config.limitingMagnitude = 12.0;  // Più deboli
// o
config.limitingMagnitude = 8.0;   // Solo stelle luminose
```

### Abilita/Disabilita Feature

```cpp
// Costellazioni
config.showConstellationLines = true;      // Linee
config.showConstellationBoundaries = false; // No confini
config.showConstellationNames = true;       // Nomi

// Numeri SAO
config.starStyle.showSAONumbers = true;
config.starStyle.minMagnitudeForLabel = 9.0; // SAO fino mag 9

// Nomi stelle
config.starStyle.showNames = true;
config.starStyle.minMagnitudeForLabel = 3.0; // Nomi solo stelle luminose
```

### Cambia Colori

```cpp
// Sfondo bianco, stelle nere (stampa)
config.backgroundColor = 0xFFFFFFFF;
config.starStyle.defaultColor = 0x000000FF;
config.gridStyle.color = 0xCCCCCCFF;

// Dark mode (schermo)
config.backgroundColor = 0x000000FF;
config.starStyle.defaultColor = 0xFFFFFFFF;
config.gridStyle.color = 0x404040FF;
```

### Cambia Dimensioni Output

```cpp
// HD
config.imageWidth = 1920;
config.imageHeight = 1920;

// 4K
config.imageWidth = 3840;
config.imageHeight = 3840;

// Stampa A4 (300 DPI)
config.imageWidth = 2480;  // 21 cm
config.imageHeight = 3508; // 29.7 cm
```

## File di Configurazione JSON

Alternativamente, crea/modifica `approach_full_example.json`:

```json
{
    "title": "Mia Carta Personalizzata",
    "centerRA": 85.0,
    "centerDec": 0.0,
    "fieldRadius": 20.0,
    
    "maxMagnitude": 10.0,
    
    "showConstellationLines": true,
    "showConstellationBoundaries": true,
    "showConstellationNames": true,
    
    "showSAONumbers": true,
    "saoMagnitudeLimit": 8.0,
    
    "outputPath": "mia_carta.png"
}
```

Poi:

```bash
./generate_chart mia_configurazione.json
```

## Regioni Interessanti da Provare

1. **Orione** (inverno)
   - Centro: RA 85°, Dec 0°
   - Campo: 25°
   - Stelle luminose, M42, Cintura di Orione

2. **Sagittario** (estate)
   - Centro: RA 280°, Dec -25°
   - Campo: 20°
   - Centro galattico, nebulae, ammassi

3. **Cigno** (estate)
   - Centro: RA 310°, Dec 40°
   - Campo: 20°
   - Croce del Nord, Via Lattea intensa

4. **Andromeda** (autunno)
   - Centro: RA 10°, Dec 40°
   - Campo: 15°
   - M31, M32, M110

5. **Grande Carro** (circumpolare)
   - Centro: RA 200°, Dec 55°
   - Campo: 25°
   - Parte di Ursa Major

## Performance

Su hardware moderno:
- Query Gaia: 20-100 ms (con database locale)
- Arricchimento SAO: 10-50 ms (con database locale)
- Rendering: 500-2000 ms (dipende da numero stelle)
- **Totale**: ~1-2 secondi

Con query online (senza database locale):
- Query Gaia: 500-2000 ms
- Arricchimento SAO: 5-20 secondi
- **Totale**: ~10-30 secondi

**Raccomandazione**: Usa il database locale SAO!

```bash
# Genera database (una volta sola)
python scripts/build_gaia_sao_database.py
```

## Troubleshooting

### Problema: Poche/nessuna stella con SAO

**Causa**: Database locale non disponibile

**Soluzione**:
```bash
cd /path/to/IOC_StarMap
python scripts/build_gaia_sao_database.py
# Copia database nella directory di lavoro
cp gaia_sao_xmatch.db build/examples/
```

### Problema: Costellazioni non mostrate

**Causa**: Dati costellazioni non implementati/disponibili

**Soluzione**: Verifica che `ConstellationData` sia popolato. Potrebbe richiedere file dati addizionali.

### Problema: Immagine vuota

**Causa**: Coordinate errate o campo troppo piccolo

**Soluzione**: Verifica coordinate e aumenta campo visivo:
```cpp
config.fieldOfViewWidth = 30.0;
```

### Problema: Troppo lento

**Causa**: Troppo stelle (mag limite alta)

**Soluzione**: Riduci magnitudine limite:
```cpp
config.limitingMagnitude = 8.0; // invece di 12.0
```

## Riferimenti

- Documentazione API: [../../docs/](../../docs/)
- Database SAO: [../../docs/GAIA_SAO_DATABASE.md](../../docs/GAIA_SAO_DATABASE.md)
- Altri esempi: [../](../)
- Configurazioni JSON: [../config_examples/](../config_examples/)
