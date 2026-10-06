import { initializeApp } from "https://www.gstatic.com/firebasejs/10.8.0/firebase-app.js";
    import { getDatabase, ref, onValue } from "https://www.gstatic.com/firebasejs/10.8.0/firebase-database.js";

    // 1. Firebase-konfigurasjon (hentet fra Firebase Console)
    const firebaseConfig = {
      apiKey: "AIzaSyAzQXjGidjqF_ndyUCEGCUZcQEQ9Ip-5A8", // Din API-nøkkel
      databaseURL: "https://tempmaaler-default-rtdb.europe-west1.firebasedatabase.app"
    };

    // 2. Initiell Firebase
    const app = initializeApp(firebaseConfig);
    const database = getDatabase(app);

    // 3. Referanse til databasen
    const sensorRef = ref(database, 'sensor');

    // 4. Lytt på sanntidsoppdateringer
    onValue(sensorRef, (snapshot) => {
      const data = snapshot.val();

      if (data) {
        // Hent ut verdiene og lagre i 3 variabler (konstanter)
        const temperatur = data.temperatur;
        const trykk = data.trykk;
        const tidstempel = data.tidstempel;

        // Skriv ut til konsollen for verifikasjon
        console.log("Mottatt data:", { temperatur, trykk, tidstempel });

        // Konverter Unix-tidstempel til lesbar norsk tid
        const dato = new Date(tidstempel);
        const tidFormatert = dato.toLocaleString("no-NO");

        // Oppdater HTML-elementene
        document.getElementById("temp").innerText = temperatur;
        document.getElementById("press").innerText = trykk;
        document.getElementById("tid").innerText = tidFormatert;
      }
    });