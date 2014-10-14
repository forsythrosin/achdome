module.exports = function(grunt) {
  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),
    watch: {
      files: ['assets/js/modules/*.js', 'assets/templates/*.hbs', 'assets/templates/partials/*.hbs'],
      tasks: ['browserify']
    },
    browserify: {
      dist: {
        files: {
          'assets/js/achdome.js': ['assets/js/modules/main.js'],
        },
        options: {
          transform: ['browserify-handlebars']
        }
      }
    }
  });

  grunt.loadNpmTasks('grunt-contrib-watch');
  grunt.loadNpmTasks('grunt-browserify');
};
