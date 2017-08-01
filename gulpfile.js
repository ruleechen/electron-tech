/*
* gulpfile
*/

const gulp = require('gulp');
const install = require('gulp-install');

gulp.task('build-window-binding', () => (
  gulp.src('./src/components/window-binding/package.json')
    .pipe(install({ production: true }))
));

gulp.task('build-connect-netsdk', () => (
  gulp.src('./src/components/connect-netsdk/package.json')
    .pipe(install({ production: true }))
));

gulp.task('build', ['build-connect-netsdk', 'build-window-binding'], () => {
});
