/*
* gulpfile
*/

const gulp = require('gulp');
const install = require('gulp-install');
const electronInstaller = require('electron-winstaller');

const isWindows = /^win/.test(process.platform);

gulp.task('build-window-binding', () => (
  gulp.src('./src/components/window-binding/package.json')
    .pipe(install({ production: true }))
));

gulp.task('build-connect-netsdk', () => (
  gulp.src('./src/components/connect-netsdk/package.json')
    .pipe(install({ production: true }))
));

gulp.task('build-winstaller', (callback) => {
  electronInstaller.createWindowsInstaller({
    appDirectory: '/',
    outputDirectory: '/tmp/build/installer64',
    authors: 'My App Inc.',
    exe: 'myapp.exe',
  }).then(() => {
    console.log('It worked!');
    callback();
  }).catch((ex) => {
    console.log(`No dice: ${ex.message}`);
  });
});

const components = ['build-window-binding'];
if (isWindows) {
  components.push('build-connect-netsdk');
}

gulp.task('build', components, () => {
});
