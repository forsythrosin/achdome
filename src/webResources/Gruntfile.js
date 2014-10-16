module.exports = function(grunt) {
  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),
    watch: {
      browserify: {
        files: ['assets/js/modules/*.js', 'assets/templates/*.hbs', 'assets/templates/partials/*.hbs'],
        tasks: ['browserify']
      },
      copy: {
        files: ['*'],
        tasks: ['copy']
      }
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
    },
    copy: {
      main: {
        src: ['./**/*', '!./**/node_modules/**'],
        expand: true,
        cwd: './',
        dest: '../../build/webResources/'
      }
    }
  });

  grunt.registerTask('default', ['browserify']);
  grunt.loadNpmTasks('grunt-contrib-watch');
  grunt.loadNpmTasks('grunt-contrib-copy');
  grunt.loadNpmTasks('grunt-browserify');
};
