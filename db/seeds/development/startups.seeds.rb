require_relative 'helper'

after 'development:levels', 'development:categories' , 'development:faculty'do
  puts 'Seeding startups'

  # Load levels.
  level_0 = Level.zero
  startup_course_level_1 = Level.find_by(name: 'Admissions')
  startup_course_level_2 = Level.find_by(name: 'Wireframe')
  developer_course_level_1 = Level.find_by(name: 'Planning')
  vr_course_level_1 = Level.find_by(name: 'New Realities')
  ios_course_level_1 = Level.find_by(name: 'iOS First Level')

  # Level 0 startup.
  Startup.create!(
    product_name: 'Unfinished Swan',
    level: level_0,
  )

  # Startup with live agreement.
  Startup.create!(
    level: startup_course_level_1,
    product_name: 'Super Product',
    product_description: 'This really is a superb product! ;)',
    agreement_signed_at: 18.months.ago,
    website: 'https://www.superstartup.in',
    logo: File.open(File.join(Rails.root, 'app/assets/images/mailer/logo-mailer.png')),
    presentation_link: 'https://slideshare.net/superstartupdeck',
    legal_registered_name: 'SuperTech Ltd',
    startup_categories: [StartupCategory.first, StartupCategory.second],
    twitter_link: 'https://twitter.com/superstartup',
    facebook_link: 'https://facebook.com/superstartup',
    product_video_link: 'https://www.youtube.com/ourvideo',
    prototype_link: 'https://www.github.com/superstartup',
    wireframe_link: 'https://drive.google.com/superstartup/wireframe',
    program_started_on: 8.weeks.ago,
  )

  # A second 'Avengers' startup.
  Startup.create!(
    name: 'The Avengers',
    level: startup_course_level_2,
    product_name: 'The Avengers',
    product_description: 'Earths Mightiest Heroes joined forces to take on threats that were too big for any one hero to tackle.',
    agreement_signed_at: 2.years.ago,
    website: 'https://www.example.org',
    startup_categories: [StartupCategory.second, StartupCategory.last],
    program_started_on: 4.weeks.ago
  )

  # Third startup 'Justice League' for developer course
  Startup.create!(
    name: 'Justice League',
    level: developer_course_level_1,
    product_name: 'Justice League',
    product_description: 'The flying car',
    agreement_signed_at: 2.years.ago,
    website: 'https://www.example.org',
    startup_categories: [StartupCategory.first, StartupCategory.last],
    program_started_on: 2.weeks.ago
  )

  # Fourth startup 'Guardians of the Galaxy' for VR course
  Startup.create!(
    name: 'Guardians of the Galaxy',
    level: vr_course_level_1,
    product_name: 'Guardians of the Galaxy',
    product_description: 'The Quad Blasters are Star-Lords primary weapons in combat.',
    agreement_signed_at: 1.years.ago,
    website: 'https://www.example.org',
    startup_categories: [StartupCategory.first, StartupCategory.last],
    program_started_on: 1.weeks.ago
  )

  # Add a startup in iOS course.
  ios_startup = Startup.create!(
    name: 'iOS Startup',
    level: ios_course_level_1,
    product_name: 'iOS Startup',
    product_description: 'This is an example iOS Startup.',
  )

  # Add a faculty to iOS Course.
  ios_startup.faculty << Faculty.find_by(name: 'iOS Coach')

  # Add a faculty to Avengers Startup
  Startup.find_by(product_name: 'The Avengers').faculty << Faculty.find_by(name: 'Vishnu Gopal')
end
