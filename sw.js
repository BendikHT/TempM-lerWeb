const CACHE_NAME = 'min-app-cache-v2'; // øk tallet når du vil tvinge ny cache
const FILES_TO_CACHE = [
  './',
  './index.html',
  './style.css',
  './app.js',
  './manifest.json'
];

// 1. Installer og lagre filer i cache
self.addEventListener('install', (event) => {
  self.skipWaiting(); // ta over med en gang
  event.waitUntil(
    caches.open(CACHE_NAME).then((cache) => cache.addAll(FILES_TO_CACHE))
  );
});

// 2. Slett gamle cacher og ta kontroll over åpne sider
self.addEventListener('activate', (event) => {
  event.waitUntil(
    caches.keys().then((keys) =>
      Promise.all(
        keys.filter((k) => k !== CACHE_NAME).map((k) => caches.delete(k))
      )
    ).then(() => self.clients.claim())
  );
});

// 3. Nettverk først, cache som reserve
self.addEventListener('fetch', (event) => {
  const url = new URL(event.request.url);

  // Ikke rør Firebase, gstatic eller andre eksterne forespørsler
  if (event.request.method !== 'GET' || url.origin !== self.location.origin) {
    return;
  }

  event.respondWith(
    fetch(event.request)
      .then((response) => {
        const kopi = response.clone();
        caches.open(CACHE_NAME).then((cache) => cache.put(event.request, kopi));
        return response;
      })
      .catch(() => caches.match(event.request))
  );
});